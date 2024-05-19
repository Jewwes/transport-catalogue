#include "request_handler.h"

void RequestHandler::ProcessRequests(const json::Node& requests) const {
    json::Array result;
    const json::Array& arr = requests.AsArray();
    for (auto& request : arr) {
        const auto& request_map = request.AsMap();
        const auto& type = request_map.at("type").AsString();
        if (type == "Stop") {
            result.emplace_back(PrintStop(request_map).AsMap());
        }
        if (type == "Bus") {
            result.emplace_back(PrintRoute(request_map).AsMap());
        }
    }
    json::Print(json::Document{ result }, std::cout);
}

const json::Node RequestHandler::PrintRoute(const json::Dict& request) const {
    json::Dict result;
    result["request_id"] = request.at("id").AsInt();
    if (!catalogue_.FindRoute(request.at("name").AsString())) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        result["curvature"] = catalogue_.GetBusStat(request.at("name").AsString())->curvature;
        result["route_length"] = catalogue_.GetBusStat(request.at("name").AsString())->route_length;
        result["stop_count"] = static_cast<int>(catalogue_.GetBusStat(request.at("name").AsString())->stops_count);
        result["unique_stop_count"] = static_cast<int>(catalogue_.GetBusStat(request.at("name").AsString())->unique_stops_count);
    }
    return json::Node{ result };
}

const json::Node RequestHandler::PrintStop(const json::Dict& request) const {
    json::Dict result;
    const std::string& stop_name = request.at("name").AsString();
    result["request_id"] = request.at("id").AsInt();
    if (!catalogue_.FindStop(stop_name)) {
        result["error_message"] = json::Node{ static_cast<std::string>("not found") };
    }
    else {
        json::Array buses;
        for (auto& bus : catalogue_.FindStop(stop_name)->buses_by_stop) {
            buses.push_back(bus);
        }
        result["buses"] = buses;
    }

    return json::Node{ result };
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.AllBuses());
}
