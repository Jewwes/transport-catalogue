#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "transport_router.h"
#include "map_renderer.h"

#include <optional>


class RequestHandler {
public:
    explicit RequestHandler(const transport::TransportCatalogue& catalogue, const map_renderer::MapRenderer& renderer, const router::Router& router)
        : catalogue_(catalogue)
        , renderer_(renderer)
        , router_(router)
    {
    }
    svg::Document RenderMap() const;
    const transport::TransportCatalogue& GetCatalogue() const;
    const router::Router& GetRouter() const;
private:
    const transport::TransportCatalogue& catalogue_;
    const map_renderer::MapRenderer& renderer_;
    const router::Router& router_;
};