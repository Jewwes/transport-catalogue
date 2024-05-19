#include "json_reader.h"

JsonReader::JsonReader(std::istream & input)
    : input_(json::Load(input))
{
}

const json::Node& JsonReader::GetRequest(std::string request) const{
    return input_.GetRoot().AsMap().at(request);
}

void JsonReader::FillCatalogue(transport::TransportCatalogue& catalogue) {
    const json::Array& arr = input_.GetRoot().AsMap().at("base_requests").AsArray();
    for (auto& request_stops : arr) {
        const auto& request_stops_map = request_stops.AsMap();
        const auto& type = request_stops_map.at("type").AsString();
        if (type == "Stop") {
            FillStop(request_stops_map, catalogue);
        }
    }
    for (auto& request_stops_distances : arr) {
        const auto& request_stops_distances_map = request_stops_distances.AsMap();
        const auto& type = request_stops_distances_map.at("type").AsString();
        if (type == "Stop" && request_stops_distances.AsMap().count("road_distances")) {
            FillStopDistances(request_stops_distances_map, catalogue);
        }
    }

    for (auto& request_bus : arr) {
        const auto& type = request_bus.AsMap().at("type").AsString();
        if (type == "Bus") {
            FillRoute(request_bus.AsMap(), catalogue);
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
        for (const auto& stop_dist : request.at("road_distances").AsMap()) {
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

void JsonReader::ProcessRequests(const json::Node& requests, RequestHandler& dos) const {
    json::Array result;
    const json::Array& arr = requests.AsArray();
    for (auto& request : arr) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.emplace_back(PrintStop(request_map,dos).AsMap());
        }
        if (type == "Bus") {
            result.emplace_back(PrintRoute(request_map,dos).AsMap());
        }
        if (type == "Map") {
            result.emplace_back(PrintMap(request_map,dos).AsMap());
        }
    }
    json::Print(json::Document{ result }, std::cout);
}

const json::Node JsonReader::PrintRoute(const json::Dict& request, RequestHandler& dos) const {
    json::Dict result;
    result["request_id"] = request.at("id").AsInt();
    if (!dos.GetCatalogue().FindRoute(request.at("name").AsString())) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = dos.GetBusStat(request.at("name").AsString())->curvature;
        result["route_length"] = dos.GetBusStat(request.at("name").AsString())->route_length;
        result["stop_count"] = static_cast<int>(dos.GetBusStat(request.at("name").AsString())->stops_count);
        result["unique_stop_count"] = static_cast<int>(dos.GetBusStat(request.at("name").AsString())->unique_stops_count);
    }
    return json::Node{ result };
}

const json::Node JsonReader::PrintStop(const json::Dict& request, RequestHandler& dos) const {
    json::Dict result;
    const std::string& stop_name = request.at("name").AsString();
    result["request_id"] = request.at("id").AsInt();
    if (!dos.GetCatalogue().FindStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : dos.GetCatalogue().FindStop(stop_name)->buses_by_stop) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

const json::Node JsonReader::PrintMap(const json::Dict& request, RequestHandler& dos) const {
    std::ostringstream out;
    svg::Document mapa = dos.RenderMap();
    json::Dict result;
    result["request_id"] = request.at("id").AsInt();
    mapa.Render(out);
    result["map"] = out.str();
    return json::Node{ result };
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
