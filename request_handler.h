#pragma once

#include "json.h"
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
    std::optional<transport::BusStat> GetBusStat(const std::string_view& bus_number) const;
    svg::Document RenderMap() const;
    const transport::TransportCatalogue& GetCatalogue() const;
private:
    const transport::TransportCatalogue& catalogue_;
    const map_renderer::MapRenderer& renderer_;
};
