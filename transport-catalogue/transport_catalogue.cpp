#include "transport_catalogue.h"
#include <iostream>
#include <algorithm>
#include <vector>
    namespace TransportCatalogue {
    void TransportCatalogue::AddStop(const std::string& name, const Coordinates& coordinates) {
        Stop* new_stop = new Stop{name, coordinates};
        stops_[name] = new_stop;
        stop_names_to_add_.insert(name);
        stop_index_[new_stop] = name; 
    }

    void TransportCatalogue::AddRoute(const std::string& name, const std::deque<std::string>& stops, bool isCircular) {
        Bus* bus = new Bus{name, stops, isCircular};
        buses_[name] = bus;
        bus_names_to_add_.insert(name);
    }

    const Stop* TransportCatalogue::FindStop(const std::string& name) const {
        auto it = stops_.find(name);
        if (it != stops_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    const Bus* TransportCatalogue::FindRoute(const std::string& name) const {
        auto it = buses_.find(name);
        if (it != buses_.end()) {
            return it->second;
        } else {
            return nullptr;
        }
    }

    void TransportCatalogue::GetRouteInfo(const std::string& bus_name) const {
        const Bus* bus = FindRoute(bus_name);
        if (bus) {
            int num_stops = bus->stops.size();
            int num_unique_stops = std::unordered_set<std::string>(bus->stops.begin(), bus->stops.end()).size();
            
            double route_length = 0.0;
            for (size_t i = 0; i < bus->stops.size() - 1; ++i) {
                const Stop* stop1 = FindStop(bus->stops[i]);
                const Stop* stop2 = FindStop(bus->stops[i + 1]);
                double distance = ComputeDistance(stop1->coordinates, stop2->coordinates);
                route_length += distance;
            }
            
            if (bus->isCircular && num_stops > 0) {
                const Stop* first_stop = FindStop(bus->stops.front());
                const Stop* last_stop = FindStop(bus->stops.back());
                double distance = ComputeDistance(last_stop->coordinates, first_stop->coordinates);
                route_length += distance;
            }
            
            std::cout << "Bus " << bus_name << ": " << num_stops << " stops on route, " 
                      << num_unique_stops << " unique stops, " << route_length << " route length" << std::endl;
        } else {
            std::cout << "Bus " << bus_name << ": not found" << std::endl;
        }
    }

    void TransportCatalogue::GetStopInfo(const std::string& request) const {
        const Stop* stop = FindStop(request);
        if (stop) {
            std::cout << "Stop " << stop->name << ":";
            
            std::vector<std::string> buses;
            for (const auto& buses_pair : buses_) {
                const Bus* bus = buses_pair.second;
                if (std::find(bus->stops.begin(), bus->stops.end(), stop->name) != bus->stops.end()) {
                    buses.push_back(bus->name); 
                }
            }
            std::sort(buses.begin(), buses.end());
            
            if (!buses.empty()) {
                std::cout<< " buses";
                for (const std::string& bus : buses) {
                    std::cout << " " << bus;
                }
            } else {
                std::cout << " no buses";
            }
            std::cout << std::endl;
        } else {
            std::cout << "Stop " << request << ": not found" << std::endl;
        }
    }
}
