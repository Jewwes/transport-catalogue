#pragma once

#include "router.h"
#include "transport_catalogue.h"

#include <memory>

namespace router{

struct RouterInfo{
    int bus_wait_time = 0;
    int bus_velocity = 0;
};
    
class Router{
public:
	Router(const int bus_wait_time, const double bus_velocity)
		: route_info_(bus_wait_time,bus_velocity) {}

	Router(const Router& settings, const transport::TransportCatalogue& catalogue) {
		route_info_.bus_wait_time = settings.route_info_.bus_wait_time;
		route_info_.bus_velocity = settings.route_info_.bus_velocity;
		BuildGraph(catalogue);
	}
    
    const graph::DirectedWeightedGraph<double>& BuildGraph(const transport::TransportCatalogue& catalogue);
    const std::optional<graph::Router<double>::RouteInfo> BuildRoute(const std::string_view from, const std::string_view to) const;
    const graph::DirectedWeightedGraph<double>& GetGraph() const;    
private:
    RouterInfo route_info_;
    std::map<std::string_view,graph::VertexId> vertex_ids_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
};
    
}