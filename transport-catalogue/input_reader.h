#pragma once
#include "transport_catalogue.h"

namespace Catalogue {

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
            /**
             * Парсит строку в структуру CommandDescription и сохраняет результат в commands_
             */
            void ParseLine(std::string_view line);
            /**
             * Наполняет данными транспортный справочник, используя команды из commands_
             */
            void ApplyCommands(TransportCatalogue& catalogue) const;
            static void ReadInput(std::istream& input_stream, TransportCatalogue& catalogue);
        private:
            std::vector<CommandDescription> commands_;
        };
} 
}
