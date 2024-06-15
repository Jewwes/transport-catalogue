#include "json_reader.h"
#include "request_handler.h"

int main() {
    transport::TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetRequest("stat_requests");
    const auto& render_settings = json_doc.GetRequest("render_settings").AsDict();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    const router::Router router = { json_doc.FillRouterData(json_doc.GetRequest("routing_settings")), catalogue };
    
    RequestHandler rh(catalogue, renderer, router);
    json_doc.ProcessRequests(stat_requests, rh);
}