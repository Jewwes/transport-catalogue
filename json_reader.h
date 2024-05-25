#pragma once

#include "json.h"
#include "transport_catalogue.h"
#include "request_handler.h"
#include "map_renderer.h"
#include "json_builder.h"
#include <iostream>
#include <sstream>
class JsonReader {
public:
    JsonReader(std::istream& input);
    const json::Node& GetRequest(std::string request) const;
    void FillCatalogue(transport::TransportCatalogue& catalogue);
    void ProcessRequests(const json::Node& requests, RequestHandler& dos) const;
    const json::Node PrintStop(const json::Dict& request, RequestHandler& dos) const;
    const json::Node PrintRoute(const json::Dict& request, RequestHandler& dos) const;
    const json::Node PrintMap(const json::Dict& request, RequestHandler& dos) const;
    map_renderer::MapRenderer FillRenderSettings(const json::Dict& request_map) const;
private:
    json::Document input_;

    void FillStop(const json::Dict& request_map, transport::TransportCatalogue& catalogue) const;
    void FillStopDistances(const json::Dict& request, transport::TransportCatalogue& catalogue) const;
    void FillRoute(const json::Dict& request_map, transport::TransportCatalogue& catalogue) const;
};
