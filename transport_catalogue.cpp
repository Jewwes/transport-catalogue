#include "transport_catalogue.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <optional>

namespace transport {

    void TransportCatalogue::AddStop(std::string_view& stop_name, const geo::Coordinates& coordinates) {
        stops_.push_back({ std::string(stop_name), coordinates, {} });
        stops_name_[stops_.back().name] = &stops_.back();
    }

    void TransportCatalogue::AddRoute(std::string_view& bus_number, const std::vector<const Stop*>& stops, bool is_circle) {
        buses_.push_back({ std::string(bus_number), stops, is_circle });
        buses_name_[buses_.back().name] = &buses_.back();
        for (const auto& route_stop : stops) {
            for (auto& stop_ : stops_) {
                if (stop_.name == route_stop->name) stop_.buses_by_stop.insert(std::string(bus_number));
            }
        }
    }

    std::optional<transport::BusStat> TransportCatalogue::GetBusStat(const std::string_view& bus_number) const {
        transport::BusStat bus_stat{};
        std::unordered_set<const transport::Stop*> unique_stops;
        for (const auto* stop : FindRoute(bus_number)->stops) {
            unique_stops.insert(stop);
        }
        size_t num_unique_stops = unique_stops.size();
        const transport::Bus* bus = FindRoute(bus_number);
        if (bus->is_circle) bus_stat.stops_count = bus->stops.size();
        else bus_stat.stops_count = bus->stops.size() * 2 - 1;
        int route_length = 0;
        double geographic_length = 0.0;
        for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
            auto from = bus->stops[i];
            auto to = bus->stops[i + 1];
            if (bus->is_circle) {
                route_length += GetDistance(from, to);
                geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates);
            }
            else {
                route_length += GetDistance(from, to) + GetDistance(to, from);
                geographic_length += geo::ComputeDistance(from->coordinates, to->coordinates) * 2;
            }
        }
        bus_stat.unique_stops_count = num_unique_stops;
        bus_stat.route_length = route_length;
        bus_stat.curvature = route_length / geographic_length;
        return bus_stat;
    }
    
    const std::map<std::string_view, const Bus*> TransportCatalogue::AllBuses() const {
        std::map<std::string_view, const Bus*> result{ buses_name_.begin(),buses_name_.end() };
        return result;
    }

    const std::map<std::string_view, const Stop*> TransportCatalogue::AllStops() const {
        std::map<std::string_view, const Stop*> result{ stops_name_.begin(),stops_name_.end() };
        return result;
    }
    
    const Bus* TransportCatalogue::FindRoute(const std::string_view& name) const {
        return buses_name_.count(std::string(name)) ? buses_name_.at(std::string(name)) : nullptr;
    }

    const Stop* TransportCatalogue::FindStop(const std::string_view& name) const {
        return stops_name_.count(std::string(name)) ? stops_name_.at(std::string(name)) : nullptr;
    }

    void TransportCatalogue::AddDistance(const Stop* from, const Stop* to, const int distance) {
        distances_[{from, to}] = distance;
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

}