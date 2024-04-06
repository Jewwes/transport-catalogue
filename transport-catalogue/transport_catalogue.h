#pragma once

#include <string>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include "geo.h"

namespace TransportCatalogue {

    struct Stop {
        std::string name;
        Coordinates coordinates;
    };

    struct Bus {
        std::string name;
        std::deque<std::string> stops;
        bool isCircular;
    };

    class TransportCatalogue {
    public:
        void AddStop(const std::string& name, const Coordinates& coordinates);
        void AddRoute(const std::string& name, const std::deque<std::string>& stops, bool isCircular);
        const Stop* FindStop(const std::string& name) const;
        const Bus* FindRoute(const std::string& name) const;
        void GetRouteInfo(const std::string& route_name) const;
        void GetStopInfo(const std::string& request) const;

    private:
        std::unordered_map<std::string, const Stop*> stops_;
        std::unordered_map<std::string, const Bus*> buses_;
        std::unordered_set<std::string> stop_names_to_add_;
        std::unordered_set<std::string> bus_names_to_add_;
        std::unordered_map<const Stop*, std::string> stop_index_;
    };

}
