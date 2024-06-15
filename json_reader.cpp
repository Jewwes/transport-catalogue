#include "json_reader.h"

JsonReader::JsonReader(std::istream & input)
    : input_(json::Load(input))
{
}

const json::Node& JsonReader::GetRequest(std::string request) const{
    return input_.GetRoot().AsDict().at(request);
}

void JsonReader::FillCatalogue(transport::TransportCatalogue& catalogue) {
    const json::Array& arr = input_.GetRoot().AsDict().at("base_requests").AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsDict();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            FillStop(request_stops_map, catalogue);
        }
    }
    for (auto& request_stops_distances : arr) {
        const auto& request_stops_distances_map = request_stops_distances.AsDict();
        const auto& type = request_stops_distances_map.at("type").AsString();
        if (type == "Stop" && request_stops_distances.AsDict().count("road_distances")) {
            FillStopDistances(request_stops_distances_map, catalogue);
        }
    }

    for (auto& request_bus : arr) {
        const auto& type = request_bus.AsDict().at("type").AsString();
        if (type == "Bus") {
            FillRoute(request_bus.AsDict(), catalogue);
        }
    }
}

void JsonReader::FillStop(const json::Dict& request, transport::TransportCatalogue& catalogue) const {
    std::string_view stop_name = request.at("name").AsString();
    geo::Coordinates coordinates = { request.at("latitude").AsDouble(), request.at("longitude").AsDouble() };
    catalogue.AddStop(stop_name, coordinates);
}

void JsonReader::FillStopDistances(const json::Dict& request, transport::TransportCatalogue& catalogue) const {
       std::string name = request.at("name").AsString();
        for (const auto& stop_dist : request.at("road_distances").AsDict()) {
            catalogue.AddDistance( catalogue.FindStop(name), catalogue.FindStop(stop_dist.first) , stop_dist.second.AsInt());
        }
}

void JsonReader::FillRoute(const json::Dict& request, transport::TransportCatalogue& catalogue) const {
    std::string_view bus = request.at("name").AsString();
    std::vector<const transport::Stop*> stops;
    for (auto& stop : request.at("stops").AsArray()) {
        stops.push_back(catalogue.FindStop(stop.AsString()));
    }
    bool circular_route = request.at("is_roundtrip").AsBool();

    catalogue.AddRoute(bus, stops, circular_route);
}

router::RouterInfo JsonReader::FillRouterData(const json::Node& data) const{
    return router::RouterInfo(data.AsDict().at("bus_wait_time").AsInt(), data.AsDict().at("bus_velocity").AsDouble());
}

map_renderer::MapRenderer JsonReader::FillRenderSettings(const json::Dict& request) const {
    map_renderer::RenderSettings render_settings;
    render_settings.width = request.at("width").AsDouble();
    render_settings.height = request.at("height").AsDouble();
    render_settings.padding = request.at("padding").AsDouble();
    render_settings.stop_radius = request.at("stop_radius").AsDouble();
    render_settings.line_width = request.at("line_width").AsDouble();
    render_settings.bus_label_font_size = request.at("bus_label_font_size").AsInt();
    const json::Array& bus_label_offset = request.at("bus_label_offset").AsArray();
    render_settings.bus_label_offset = { bus_label_offset[0].AsDouble(), bus_label_offset[1].AsDouble() };
    render_settings.stop_label_font_size = request.at("stop_label_font_size").AsInt();
    const json::Array& stop_label_offset = request.at("stop_label_offset").AsArray();
    render_settings.stop_label_offset = { stop_label_offset[0].AsDouble(), stop_label_offset[1].AsDouble() };

    if (request.at("underlayer_color").IsString()) {
        render_settings.underlayer_color = request.at("underlayer_color").AsString(); 
    }
    else if (request.at("underlayer_color").IsArray()) {
        const json::Array& underlayer_color = request.at("underlayer_color").AsArray();
        if (underlayer_color.size() == 3) {
            render_settings.underlayer_color = svg::Rgb(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt());
        }
        else if (underlayer_color.size() == 4) {
            render_settings.underlayer_color = svg::Rgba(underlayer_color[0].AsInt(), underlayer_color[1].AsInt(), underlayer_color[2].AsInt(), underlayer_color[3].AsDouble());
        }
    }

    render_settings.underlayer_width = request.at("underlayer_width").AsDouble();

    const json::Array& color_palette = request.at("color_palette").AsArray();
    for (const auto& color_element : color_palette) {
        if (color_element.IsString()) {
            render_settings.color_palette.push_back(color_element.AsString());
        }
        else if (color_element.IsArray()) {
            const json::Array& color_type = color_element.AsArray();
            if (color_type.size() == 3) {
                render_settings.color_palette.emplace_back(svg::Rgb(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt()));
            }
            else if (color_type.size() == 4) {
                render_settings.color_palette.emplace_back(svg::Rgba(color_type[0].AsInt(), color_type[1].AsInt(), color_type[2].AsInt(), color_type[3].AsDouble()));
            }
        }
    }

    return render_settings;
}

void JsonReader::ProcessRequests(const json::Node& requests, RequestHandler& dos) const {
    json::Array result;
    const json::Array& arr = requests.AsArray();
    for (auto& request : arr) {
        const auto& request_map = request.AsDict();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.emplace_back(PrintStop(request_map,dos).AsDict());
        }
        if (type == "Bus") {
            result.emplace_back(PrintRoute(request_map,dos).AsDict());
        }
        if (type == "Map") {
            result.emplace_back(PrintMap(request_map,dos).AsDict());
        }
        if (type == "Route") {
            result.emplace_back(PrintRouter(request_map,dos).AsDict());
        }
    }
    json::Print(json::Document{ result }, std::cout);
}

const json::Node JsonReader::PrintRoute(const json::Dict& request, RequestHandler& dos) const {
    json::Node result;
    if (!dos.GetCatalogue().FindRoute(request.at("name").AsString())) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(request.at("id").AsInt())
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(request.at("id").AsInt())
            .Key("curvature").Value(dos.GetCatalogue().GetBusStat(request.at("name").AsString())->curvature)
            .Key("route_length").Value(dos.GetCatalogue().GetBusStat(request.at("name").AsString())->route_length)
            .Key("stop_count").Value(static_cast<int>(dos.GetCatalogue().GetBusStat(request.at("name").AsString())->stops_count))
            .Key("unique_stop_count").Value(static_cast<int>(dos.GetCatalogue().GetBusStat(request.at("name").AsString())->unique_stops_count))
            .EndDict()
            .Build();
    }
    return result;
}

const json::Node JsonReader::PrintStop(const json::Dict& request, RequestHandler& dos) const {
    json::Node result;
    if (!dos.GetCatalogue().FindStop(request.at("name").AsString())) {
        result = json::Builder{}
            .StartDict()
            .Key("request_id").Value(request.at("id").AsInt())
            .Key("error_message").Value("not found")
            .EndDict()
            .Build();
    }
    else {
        json::Array buses;
        for (auto& bus : dos.GetCatalogue().FindStop(request.at("name").AsString())->buses_by_stop) {
            buses.push_back(bus);
        }
        result = json::Builder{}.
            StartDict()
            .Key("request_id").Value(request.at("id").AsInt())
            .Key("buses").Value(buses)
            .EndDict()
            .Build();
    }

    return result;
}

const json::Node JsonReader::PrintMap(const json::Dict& request, RequestHandler& dos) const {
    json::Node result;
    const int id = request.at("id").AsInt();
    std::ostringstream strm;
    svg::Document map = dos.RenderMap();
    map.Render(strm);
    
    result = json::Builder{}
                .StartDict()
                    .Key("request_id").Value(id)
                    .Key("map").Value(strm.str())
                .EndDict()
            .Build();
    
    return result;
}

const json::Node JsonReader::PrintRouter(const json::Dict& request, RequestHandler& dos) const {
    json::Node result;
    const int id = request.at("id").AsInt();
    const auto& routing_result = dos.GetRouter().BuildRoute(request.at("from").AsString(), request.at("to").AsString());

    if (!routing_result.route_info.has_value()) {
        result = json::Builder{}
                    .StartDict()
                        .Key("request_id").Value(id)
                        .Key("error_message").Value("not found")
                    .EndDict()
                .Build();
    } else {
        json::Array items;
        for (const auto& edge : routing_result.edges) {
            if (edge.span_count == 0) {
                items.emplace_back(json::Node(json::Builder{}
                                                .StartDict()
                                                    .Key("stop_name").Value(edge.name)
                                                    .Key("time").Value(edge.weight)
                                                    .Key("type").Value("Wait")
                                                .EndDict()
                                            .Build()));
            } else {
                items.emplace_back(json::Node(json::Builder{}
                                                .StartDict()
                                                    .Key("bus").Value(edge.name)
                                                    .Key("span_count").Value(static_cast<int>(edge.span_count))
                                                    .Key("time").Value(edge.weight)
                                                    .Key("type").Value("Bus")
                                                .EndDict()
                                            .Build()));
            }
        }

        result = json::Builder{}
                    .StartDict()
                        .Key("request_id").Value(id)
                        .Key("total_time").Value(routing_result.total_time)
                        .Key("items").Value(items)
                    .EndDict()
                .Build();
    }

    return result;
}


