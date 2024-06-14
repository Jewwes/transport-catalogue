#include "transport_router.h"

namespace router{

const graph::DirectedWeightedGraph<double>& Router::BuildGraph(const transport::TransportCatalogue& catalogue){
    const auto& all_stops = catalogue.AllStops();
    const auto& all_buses = catalogue.AllBuses();
    graph::DirectedWeightedGraph<double> stop_graph(all_stops.size() * 2);
    graph::VertexId vertex_id = 0;

    for (const auto& [stop_name, stop_info] : all_stops) {
        vertex_ids_[stop_info->name] = vertex_id;
        stop_graph.AddEdge({
            stop_info->name,
            0,
            vertex_id,
            ++vertex_id,
            static_cast<double>(route_info_.bus_wait_time)
        });
        ++vertex_id;
    }

    for (const auto& item : all_buses) {
        const auto& bus_info = item.second;
        const auto& stops = bus_info->stops;
        size_t stops_count = stops.size();
        for (size_t i = 0; i < stops_count; ++i) {
            for (size_t j = i + 1; j < stops_count; ++j) {
                const transport::Stop* stop_from = stops[i];
                const transport::Stop* stop_to = stops[j];
                int dist = 0;
                int dist_round = 0;
                for (size_t k = i + 1; k <= j; ++k) {
                    dist += catalogue.GetDistance(stops[k - 1], stops[k]);
                    dist_round += catalogue.GetDistance(stops[k], stops[k - 1]);
                }
                stop_graph.AddEdge({
                    bus_info->name,
                    j - i,
                    vertex_ids_.at(stop_from->name) + 1,
                    vertex_ids_.at(stop_to->name),
                    static_cast<double>(dist) / (route_info_.bus_velocity * (100.0 / 6.0))
                });

                if (!bus_info->is_circle) {
                    stop_graph.AddEdge({
                        bus_info->name,
                        j - i,
                        vertex_ids_.at(stop_to->name) + 1,
                        vertex_ids_.at(stop_from->name),
                        static_cast<double>(dist_round) / (route_info_.bus_velocity * (100.0 / 6.0))
                    });
                }
            }
        }
    }

    graph_ = std::move(stop_graph);
    router_ = std::make_unique<graph::Router<double>>(graph_);
    return graph_;
}

const std::optional<graph::Router<double>::RouteInfo> Router::BuildRoute(const std::string_view from, const std::string_view to) const {
    return router_->BuildRoute(vertex_ids_.at(from), vertex_ids_.at(to));
}

const graph::DirectedWeightedGraph<double>& Router::GetGraph() const {
    return graph_;
}

}
