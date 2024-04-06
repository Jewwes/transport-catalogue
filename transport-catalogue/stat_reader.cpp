#include <ostream>
#include "transport_catalogue.h"
namespace TransportCatalogue {

namespace detail {

        void ParseAndPrintStat(const TransportCatalogue& transport_catalogue, std::string_view request,
                               std::ostream& output) {
            if (request.find("Bus") == 0) {
                std::string route_name(request.substr(4)); 
                transport_catalogue.GetRouteInfo(route_name);
            } else if (request.find("Stop") == 0) {
                std::string stop_name(request.substr(5)); 
                transport_catalogue.GetStopInfo(stop_name);
            }
            else {
                output << "Invalid request: " << request << std::endl;
            }
        }
} 

} 
