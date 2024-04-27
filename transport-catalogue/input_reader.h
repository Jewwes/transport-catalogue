#pragma once
#include "transport_catalogue.h"

namespace TransportCatalogue {

namespace detail {

        struct CommandDescription {
            // Определяет, задана ли команда (поле command непустое)
            explicit operator bool() const {
                return !command.empty();
            }

            bool operator!() const {
                return !operator bool();
            }

            std::string command;      // Название команды
            std::string id;           // id маршрута или остановки
            std::string description;  // Параметры команды
        };
        class InputReader {
        public:
            void ParseLine(std::string_view line);
            void ApplyCommands(TransportCatalogue& catalogue) const;
            static void ReadInput(std::istream& input_stream, TransportCatalogue& catalogue);
        private:
            std::vector<CommandDescription> commands_;
        };
} 
}
