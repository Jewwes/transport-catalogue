#pragma once

#include "geo.h"
#include "domain.h"
#include <deque>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <optional>
#include <map>

namespace transport {

    class TransportCatalogue {
    public:
        struct StopPairHash {
            std::size_t operator()(const std::pair<const Stop*, const Stop*>& p) const {
                std::size_t hash1 = std::hash<const void*>{}(p.first);
                std::size_t hash2 = std::hash<const void*>{}(p.second);
                return hash1 + hash2 * 37;
            }
        };
        void AddStop(std::string_view& stop_name, const geo::Coordinates& coordinates);
        void AddRoute(std::string_view& bus_number, const std::vector<const Stop*>& stops, bool is_circle);
        const Stop* FindStop(const std::string_view& name) const;
        const Bus* FindRoute(const std::string_view& name) const;
        void AddDistance(const Stop* from, const Stop* to, const int distance);
        int GetDistance(const Stop* from, const Stop* to) const;
        const std::map<std::string_view, const Bus*> AllBuses() const;
    private:
        std::unordered_map<std::string_view, const Stop*> stops_name_;
        std::unordered_map<std::string_view, const Bus*> buses_name_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHash> distances_;
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
    };

}
