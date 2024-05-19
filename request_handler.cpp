#include "request_handler.h"

std::optional<transport::BusStat> RequestHandler::GetBusStat(const std::string_view& bus_number) const {
    transport::BusStat bus_stat{};
    std::unordered_set<const transport::Stop*> unique_stops;
    for (const auto* stop : catalogue_.FindRoute(bus_number)->stops) {
        unique_stops.insert(stop);
    }
    size_t num_unique_stops = unique_stops.size();
    const transport::Bus* bus = catalogue_.FindRoute(bus_number);
    if (bus->is_circle) bus_stat.stops_count = bus->stops.size();
    else bus_stat.stops_count = bus->stops.size() * 2 - 1;

    int route_length = 0;
    double geographic_length = 0.0;

    for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
        auto from = bus->stops[i];
        auto to = bus->stops[i + 1];
        if (bus->is_circle) {
            route_length += catalogue_.GetDistance(from, to);
            geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates);
        }
        else {
            route_length += catalogue_.GetDistance(from, to) + catalogue_.GetDistance(to, from);
            geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates) * 2;
        }
    }

    bus_stat.unique_stops_count = num_unique_stops;
    bus_stat.route_length = route_length;
    bus_stat.curvature = route_length / geographic_length;

    return bus_stat;
}

const transport::TransportCatalogue& RequestHandler::GetCatalogue() const{
    return catalogue_;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.AllBuses());
}
