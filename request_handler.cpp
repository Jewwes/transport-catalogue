#include "request_handler.h"

const transport::TransportCatalogue& RequestHandler::GetCatalogue() const {
    return catalogue_;
}

svg::Document RequestHandler::RenderMap() const {
    return renderer_.GetSVG(catalogue_.AllBuses());
}

const router::Router& RequestHandler::GetRouter() const {
    return router_;
}