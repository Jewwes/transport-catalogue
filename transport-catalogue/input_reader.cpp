#include "input_reader.h"
#include <algorithm>
#include <iostream>
using namespace std::literals;
namespace Catalogue {
    namespace detail {

        /**
         * Парсит строку вида "10.123,  -30.1837" и возвращает пару координат (широта, долгота)
         */
        Coordinates ParseCoordinates(std::string_view str) {
            static const double nan = std::nan("");

            auto not_space = str.find_first_not_of(' ');
            auto comma = str.find(',');

            if (comma == str.npos) {
                return { nan, nan };
            }

            auto not_space2 = str.find_first_not_of(' ', comma + 1);

            double lat = std::stod(std::string(str.substr(not_space, comma - not_space)));
            double lng = std::stod(std::string(str.substr(not_space2)));

            return { lat, lng };
        }

        /**
         * Удаляет пробелы в начале и конце строки
         */
        std::string_view Trim(std::string_view string) {
            const auto start = string.find_first_not_of(' ');
            if (start == string.npos) {
                return {};
            }
            return string.substr(start, string.find_last_not_of(' ') + 1 - start);
        }

        /**
         * Разбивает строку string на n строк, с помощью указанного символа-разделителя delim
         */
        std::vector<std::string_view> Split(std::string_view string, char delim) {
            std::vector<std::string_view> result;

            size_t pos = 0;
            while ((pos = string.find_first_not_of(' ', pos)) < string.length()) {
                auto delim_pos = string.find(delim, pos);
                if (delim_pos == string.npos) {
                    delim_pos = string.size();
                }
                if (auto substr = Trim(string.substr(pos, delim_pos - pos)); !substr.empty()) {
                    result.push_back(substr);
                }
                pos = delim_pos + 1;
            }
            return result;
        }

        /**
         * Парсит маршрут.
         * Для кольцевого маршрута (A>B>C>A) возвращает массив названий остановок [A,B,C,A]
         * Для некольцевого маршрута (A-B-C-D) возвращает массив названий остановок [A,B,C,D,C,B,A]
         */
        std::vector<std::string_view> ParseRoute(std::string_view route) {
            if (route.find('>') != route.npos) {
                return Split(route, '>');
            }

            auto stops = Split(route, '-');
            std::vector<std::string_view> results(stops.begin(), stops.end());
            results.insert(results.end(), std::next(stops.rbegin()), stops.rend());

            return results;
        }

        CommandDescription ParseCommandDescription(std::string_view line) {
            auto colon_pos = line.find(':');
            if (colon_pos == line.npos) {
                return {};
            }

            auto space_pos = line.find(' ');
            if (space_pos >= colon_pos) {
                return {};
            }

            auto not_space = line.find_first_not_of(' ', space_pos);
            if (not_space >= colon_pos) {
                return {};
            }

            return { std::string(line.substr(0, space_pos)),
                    std::string(line.substr(not_space, colon_pos - not_space)),
                    std::string(line.substr(colon_pos + 1)) };
        }


        static ParseStop ParseStopDescription(std::string_view description) {
            ParseStop result;
            auto parts = Split(description, ',');
            if (parts.size() >= 2) {
                result.coordinates = ParseCoordinates(std::string(parts[0]) + ", "s + std::string(parts[1]));
            }

            for (size_t i = 2; i < parts.size(); ++i) {
                auto distance_str = parts[i];
                auto m_to_pos = distance_str.find("m to ");
                if (m_to_pos != distance_str.npos) {
                    std::string stop_name = std::string(distance_str.substr(m_to_pos + 5));
                    int distance = std::stoi(std::string(distance_str.substr(0, m_to_pos)));
                    result.name_distance.push_back({ stop_name, distance });
                    //Добавил структуру ParseStop, чтобы в дальнейшем было понятно, что это за neighbor.first и neighbor.second
                    //Были мысли на счет того, чтобы сделать структуру в структуре, дабы за
                }
            }
            return result;
        }


        void InputReader::ParseLine(std::string_view line) {
            auto command_description = ParseCommandDescription(line);
            if (command_description) {
                commands_.push_back(std::move(command_description));
            }
        }


        void InputReader::ApplyCommands(TransportCatalogue& catalogue) const {
            for (const auto& command : commands_) {
                if (command.command == "Stop") {
                    ParseStop stop_info = ParseStopDescription(command.description);
                    catalogue.AddStop(command.id, stop_info.coordinates);
                }
            }
            for (const auto& command : commands_){
                if (command.command == "Stop") {
                    ParseStop stop_info = ParseStopDescription(command.description);
                    for (const auto& neighbor : stop_info.name_distance) {
                        catalogue.AddDistance(command.id, neighbor.first, neighbor.second);
                    }
                }
            }
            for (const auto& command : commands_) {
                if (command.command == "Bus") {
                    std::vector<std::string_view> route = ParseRoute(command.description);
                    std::vector<const Stop*> stops;
                    for (const auto& stop : route) {
                        const Stop* found_stop = catalogue.FindStop(std::string(stop));
                        if (found_stop) {
                            stops.push_back(found_stop);
                        }
                    }
                    if (!stops.empty()) {
                        bool isCircular = (stops.front() == stops.back());
                        catalogue.AddRoute(command.id, stops, isCircular);
                    }
                }
            }
        }
    
        void InputReader::ReadInput(std::istream& input_stream, TransportCatalogue& catalogue) {
            std::vector<std::string> input_lines;
            int base_request_count;
            input_stream >> base_request_count >> std::ws;
            input_lines.reserve(base_request_count);

            for (int i = 0; i < base_request_count; ++i) {
                std::string line;
                getline(input_stream, line);
                input_lines.push_back(line);
            }

            InputReader reader;
            for (const auto& line : input_lines) {
                reader.ParseLine(line);
            }
            reader.ApplyCommands(catalogue);
        }


    } // namespace detail
} // namespace TransportCatalogue
