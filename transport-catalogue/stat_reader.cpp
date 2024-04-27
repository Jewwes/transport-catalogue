#include "transport_catalogue.h"
#include <ostream>
#include <iomanip>
namespace Catalogue {

    namespace detail {

        void PrintRoute(std::string& line, TransportCatalogue& catalogue, std::ostream& out) {
            std::string bus = line.substr(1, line.npos);
            if (catalogue.FindRoute(bus)) {
                auto route = catalogue.GetRouteInfo(bus);
                out << "Bus " << bus << ": " << catalogue.GetRouteInfo(bus)->stops_count << " stops on route, "
                    << route->unique_stops_count << " unique stops, " << std::setprecision(6)
                    << route->route_length << " route length, "
                    << route->curvature << " curvature\n";
            }
            else {
                out << "Bus " << bus << ": not found\n";
            }
        }

        void PrintStop(std::string& line, TransportCatalogue& catalogue, std::ostream& out) {
            std::string stop = line.substr(1, line.npos);
            if (catalogue.FindStop(stop)) {
                auto stopInfo = catalogue.GetStopInfo(stop)->buses;
                if (!stopInfo.empty()) {
                    // Accessing stopInfo->buses safely
                    out << " buses ";
                    for (const auto& busName : stopInfo) {
                        out << busName << " ";
                    }
                    out << "\n";
                }
                else {
                    out << " no buses\n";
                }
            }
            else {
                out << "Stop " << stop << ": not found\n";
            }

        }

        void ParseAndPrintStat(std::istream& in, TransportCatalogue& catalogue, std::ostream& out) {
            size_t requests_count;
            in >> requests_count;
            for (size_t i = 0; i < requests_count; ++i) {
                std::string keyword, line;
                in >> keyword;
                std::getline(in, line);
                if (keyword == "Bus") {
                    PrintRoute(line, catalogue, out);
                }
                if (keyword == "Stop") {
                    PrintStop(line, catalogue, out);
                }
            }
        }
    }

}
