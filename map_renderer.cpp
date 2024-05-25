#include "map_renderer.h"

namespace map_renderer {

    bool IsZero(double value) {
        return std::abs(value) < EPSILON;
    }

    void MapRenderer::DrawLines(const std::map<std::string_view, const transport::Bus*>& buses, svg::Document& result, const SphereProjector& sp) const {
        size_t color_num = 0;
        size_t color_index = render_settings_.color_palette.size();
        for (const auto& [bus_number, bus] : buses) {
            if (!bus->stops.empty()) {
                std::vector<const transport::Stop*> route_stops{ bus->stops.begin(), bus->stops.end() };
                if (bus->is_circle == false) {
                    route_stops.insert(route_stops.end(), std::next(bus->stops.rbegin()), bus->stops.rend());
                }
                svg::Polyline line;
                for (const auto& stop : route_stops) {
                    line.AddPoint(sp(stop->coordinates));
                }
                line.SetStrokeColor(render_settings_.color_palette[color_num]).
                    SetFillColor(svg::NoneColor).
                    SetStrokeWidth(render_settings_.line_width).
                    SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                    SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);

                if (color_num < (color_index - 1)) {
                    ++color_num;
                }
                else {
                    color_num = 0;
                }

                result.Add(line);
            }
        }
    }

    void MapRenderer::DrawRoutes(const std::map<std::string_view, const transport::Bus*>& buses, svg::Document& result, const SphereProjector& sp) const {
        size_t color_num = 0;
        size_t color_index = render_settings_.color_palette.size();
        svg::Text text;
        svg::Text sub_text;
        for (const auto& [name, bus] : buses) {
            if (!bus->stops.empty()) {
                text.SetPosition(sp(bus->stops.at(0)->coordinates)).
                    SetOffset(render_settings_.bus_label_offset).
                    SetFontSize(render_settings_.bus_label_font_size).
                    SetFontFamily("Verdana").
                    SetFontWeight("bold").
                    SetData(bus->name).
                    SetFillColor(render_settings_.color_palette[color_num]);
                if (color_num < (color_index - 1)) {
                    ++color_num;
                }
                else {
                    color_num = 0;
                }
                sub_text.SetPosition(sp(bus->stops.at(0)->coordinates)).
                    SetOffset(render_settings_.bus_label_offset).
                    SetFontSize(render_settings_.bus_label_font_size).
                    SetFontFamily("Verdana").
                    SetFontWeight("bold").
                    SetData(bus->name).
                    SetFillColor(render_settings_.underlayer_color).
                    SetStrokeColor(render_settings_.underlayer_color).
                    SetStrokeWidth(render_settings_.underlayer_width).
                    SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                    SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
                result.Add(sub_text);
                result.Add(text);

                if (bus->is_circle == false && bus->stops[0] != bus->stops[bus->stops.size() - 1]) {
                    svg::Text text_round{ text };
                    svg::Text sub_text_round{ sub_text };
                    result.Add(sub_text_round.SetPosition(sp(bus->stops.at(bus->stops.size() - 1)->coordinates)));
                    result.Add(text_round.SetPosition(sp(bus->stops.at(bus->stops.size() - 1)->coordinates)));
                }
            }
        }
    }

    void MapRenderer::DrawCircles(const std::map<std::string_view, const transport::Stop*>& stops, svg::Document& result, const SphereProjector& sp) const {
        svg::Circle circle;
        for (const auto& [name, stop] : stops) {
            circle.SetCenter(sp(stop->coordinates)).
                SetRadius(render_settings_.stop_radius).
                SetFillColor("white");
            result.Add(circle);
        }
    }

    void MapRenderer::DrawStops(const std::map<std::string_view, const transport::Stop*>& stops, svg::Document& result, const SphereProjector& sp) const {
        svg::Text text;
        svg::Text sub_text;
        for (const auto& [name, stop] : stops) {
            text.SetPosition(sp(stop->coordinates)).
                SetOffset(render_settings_.stop_label_offset).
                SetFontSize(render_settings_.stop_label_font_size).
                SetFontFamily("Verdana").
                SetData(stop->name).
                SetFillColor("black");

            sub_text.SetPosition(sp(stop->coordinates)).
                SetOffset(render_settings_.stop_label_offset).
                SetFontSize(render_settings_.stop_label_font_size).
                SetFontFamily("Verdana").
                SetData(stop->name).
                SetFillColor(render_settings_.underlayer_color).
                SetStrokeColor(render_settings_.underlayer_color).
                SetStrokeWidth(render_settings_.underlayer_width).
                SetStrokeLineCap(svg::StrokeLineCap::ROUND).
                SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
            result.Add(sub_text);
            result.Add(text);
        }
    }



    svg::Document MapRenderer::GetSVG(const std::map<std::string_view, const transport::Bus*>& buses) const {
        svg::Document result;
        std::vector<geo::Coordinates> route_stops_coord;
        std::map<std::string_view, const transport::Stop*> stops;
        for (const auto& [name, bus] : buses) {
            if (!bus->stops.empty()) {
                for (const auto& stop : bus->stops) {
                    route_stops_coord.push_back(stop->coordinates);
                    stops[stop->name] = stop;
                }
            }
        }
        SphereProjector sp(route_stops_coord.begin(), route_stops_coord.end(), render_settings_.width, render_settings_.height, render_settings_.padding);

        DrawLines(buses, result, sp);
        DrawRoutes(buses, result, sp);
        DrawCircles(stops, result, sp);
        DrawStops(stops, result, sp);
        return result;
    }

} // namespace map_renderer
