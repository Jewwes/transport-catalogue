#include "transport_catalogue.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <optional>
namespace TransportCatalogue {

    void TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates) {
        stops_.push_back({ name, coordinates });
        stops_name_[stops_.back().name] = &stops_.back();
    }

    void TransportCatalogue::AddRoute(const std::string& name, const std::vector<const Stop*>& stops, bool isCircular) {
        Bus new_bus;
        new_bus.name = name;
        new_bus.stops.reserve(stops.size());
        for (const auto* stop_ptr : stops) {
            new_bus.stops.push_back(stop_ptr);
        }
        new_bus.isCircular = isCircular;
        buses_.push_back(std::move(new_bus));
        buses_name_[buses_.back().name] = &buses_.back();
    }

    const Bus* TransportCatalogue::FindRoute(const std::string_view& name) const {
        return buses_name_.count(std::string(name)) ? buses_name_.at(std::string(name)) : nullptr;
    }

    const Stop* TransportCatalogue::FindStop(const std::string_view& name) const {
        return stops_name_.count(std::string(name)) ? stops_name_.at(std::string(name)) : nullptr;
    }

    void TransportCatalogue::AddDistance(const std::string& from, const std::string& to, int distance) {
        const Stop* stop = FindStop(from);
        const Stop* other_stop = FindStop(to);
        if (stop && other_stop) {
            std::pair<const Stop*, const Stop*> key = { stop, other_stop };
            distances_[key] = distance;

            std::pair<const Stop*, const Stop*> reverse_key = { other_stop, stop };
            if (!distances_.count(reverse_key)) {
                distances_[reverse_key] = distance;
            }
        }
    }

    int TransportCatalogue::GetDistance(const Stop* from, const Stop* to) const {
        auto it = distances_.find({ from, to });
        if (it != distances_.end()) {
            return it->second;
        }
        it = distances_.find({ to, from });
        if (it != distances_.end()) {
            return it->second;
        }
        return 0;
    }

    std::optional<BusStat> TransportCatalogue::GetRouteInfo(const std::string_view& bus_name) const {
        BusStat bus_stat{};
        const Bus* bus = FindRoute(bus_name);
        if (bus) {
            size_t num_stops = bus->stops.size();
            std::unordered_set<const Stop*> unique_stops;
            for (const auto* stop : bus->stops) {
                unique_stops.insert(stop);
            }
            size_t num_unique_stops = unique_stops.size();

            int route_length = 0;
            double route_length_geo = 0.0;
            for (size_t i = 0; i < num_stops - 1; ++i) {
                const Stop* stop1 = bus->stops[i];
                const Stop* stop2 = bus->stops[i + 1];
                int distance = GetDistance(stop1, stop2);
                double dist = ComputeDistance(stop1->coordinates, stop2->coordinates);
                route_length_geo += dist;
                if (distance > 0) {
                    route_length += distance;
                }
            }

            if (bus->isCircular && num_stops > 0) {
                const Stop* first_stop = bus->stops.front();
                const Stop* last_stop = bus->stops.back();
                int distance = GetDistance(first_stop, last_stop);
                double dist = ComputeDistance(first_stop->coordinates, last_stop->coordinates);
                route_length_geo += dist;
                if (distance > 0) {
                    route_length += distance;
                }
            }

            double curvature = (route_length_geo == 0.0) ? 0.0 : static_cast<double>(route_length) / route_length_geo;
            bus_stat.stops_count = num_stops;
            bus_stat.unique_stops_count = num_unique_stops;
            bus_stat.route_length = route_length;
            bus_stat.curvature = curvature;
            return bus_stat;
        }
        return bus_stat;
    }

    std::optional<StopStat> TransportCatalogue::GetStopInfo(const std::string_view& request) const {
        StopStat stop_opt;
        const Stop* found_stop = FindStop(request);
        if (found_stop) {
            std::vector<std::string> buses;
            for (const auto& buses_pair : buses_name_) {
                const Bus* bus = buses_pair.second;
                if (std::find(bus->stops.begin(), bus->stops.end(), found_stop) != bus->stops.end()) {
                    buses.push_back(bus->name);
                }
            }
            std::sort(buses.begin(), buses.end());
            std::set<std::string> bu(buses.begin(), buses.end());
            stop_opt.name = request;
            stop_opt.buses = bu;
            return stop_opt;
        }
        return stop_opt;
    }
}
