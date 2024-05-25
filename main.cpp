#include "json_reader.h"
#include "request_handler.h"

int main() {
    transport::TransportCatalogue catalogue;
    JsonReader json_doc(std::cin);

    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetRequest("stat_requests");
    const auto& render_settings = json_doc.GetRequest("render_settings").AsDict();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);

    RequestHandler rh(catalogue, renderer);
    json_doc.ProcessRequests(stat_requests, rh);
}
