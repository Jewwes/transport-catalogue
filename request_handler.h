#pragma once

#include "json.h"
#include "json_reader.h"
#include "transport_catalogue.h"
#include "map_renderer.h"

#include <optional>


class RequestHandler {
public:
    explicit RequestHandler(const transport::TransportCatalogue& catalogue, const map_renderer::MapRenderer& renderer)
        : catalogue_(catalogue)
        , renderer_(renderer)
    {
    }

    void ProcessRequests(const json::Node& stat_requests) const;

    const json::Node PrintRoute(const json::Dict& request_map) const;
    const json::Node PrintStop(const json::Dict& request_map) const;


    svg::Document RenderMap() const;

private:
    const transport::TransportCatalogue& catalogue_;
    const map_renderer::MapRenderer& renderer_;
};
