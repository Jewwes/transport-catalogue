#pragma once

#include "geo.h"
#include <deque>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include <optional>
namespace Catalogue {

    struct Stop {
        std::string name;
        Coordinates coordinates;
    };
    
    struct Bus {
        std::string name;
        std::vector<const Stop*> stops;
        bool isCircular = false;
    };
    
    struct BusStat {
        size_t stops_count;
        size_t unique_stops_count;
        double route_length;
        double curvature;
    };

    struct StopStat {
        std::string name;
        std::set<std::string> buses;
    };

    struct StopPairHash { 
        std::size_t operator()(const std::pair<const Stop*, const Stop*>& p) const { 
            std::size_t hash1 = std::hash<const void*>{}(p.first); 
            std::size_t hash2 = std::hash<const void*>{}(p.second); 
            return hash1 + hash2 * 37; 
        } 
    }; 
    
    class TransportCatalogue {
    public:
        void AddStop(const std::string& name, const Coordinates& coordinates);
        void AddRoute(const std::string& name, const std::vector<const Stop*>& stops, bool isCircular);
        const Stop* FindStop(const std::string_view& name) const;
        const Bus* FindRoute(const std::string_view& name) const;
        std::optional<BusStat> GetRouteInfo(const std::string_view& route_name) const;
        std::optional<StopStat> GetStopInfo(const std::string_view& request) const;
        void AddDistance(const std::string& from, const std::string& to, int distance);
        int GetDistance(const Stop* from, const Stop* to) const;
    private:
        std::unordered_map<std::string_view, const Stop*> stops_name_;
        std::unordered_map<std::string_view, const Bus*> buses_name_;
        std::unordered_map<std::pair<const Stop*, const Stop*>, int, StopPairHash> distances_;
        std::deque<Stop> stops_;
        std::deque<Bus> buses_;
    };

}
