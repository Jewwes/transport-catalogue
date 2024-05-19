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

    const std::map<std::string_view, const Bus*> TransportCatalogue::AllBuses() const {
        std::map<std::string_view, const Bus*> result{buses_name_.begin(),buses_name_.end()};
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
