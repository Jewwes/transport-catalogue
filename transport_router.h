#pragma once

#include "router.h"
#include "transport_catalogue.h"
#include <memory>
#include <map>
#include <optional>
#include <string_view>

namespace router {

struct RouteResult {
    std::optional<graph::Router<double>::RouteInfo> route_info;
    std::vector<graph::Edge<double>> edges;
    double total_time;
};

    
struct RouterInfo {
    int bus_wait_time = 0;
    int bus_velocity = 0;
};

class Router {
public:
    Router(const RouterInfo& settings, const transport::TransportCatalogue& catalogue) {
        route_info_.bus_wait_time = settings.bus_wait_time;
        route_info_.bus_velocity = settings.bus_velocity;
        BuildGraph(catalogue);
    }

    RouteResult BuildRoute(const std::string_view from, const std::string_view to) const;
private:
    RouterInfo route_info_;
    std::map<std::string_view, graph::VertexId> vertex_ids_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unique_ptr<graph::Router<double>> router_;
    const graph::DirectedWeightedGraph<double>& BuildGraph(const transport::TransportCatalogue& catalogue);
};

}  // namespace router
