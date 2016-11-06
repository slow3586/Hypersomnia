#pragma once
#include "game/enums/render_layer.h"

#include <vector>
#include <array>

#include "augs/graphics/vertex.h"
#include "game/transcendental/entity_handle_declaration.h"
#include "game/enums/renderable_drawing_type.h"

using namespace augs;

struct state_for_drawing_camera;

class render_system {
public:
	static bool render_order_compare(const const_entity_handle& a, const const_entity_handle& b);

	void draw_entities(
		augs::vertex_triangle_buffer& output, 
		const std::vector<const_entity_handle>&, 
		const state_for_drawing_camera& in, 
		const renderable_drawing_type renderable_drawing_mode
	) const;

	template<class renderable_type>
	void draw_renderable(
		augs::vertex_triangle_buffer& output,
		const renderable_type& renderable,
		const components::transform& renderable_transform,
		const components::render& render,
		const state_for_drawing_camera& in_camera,
		const renderable_drawing_type renderable_drawing_mode
	) const {

		components::transform camera_transform;
		camera_transform = render.absolute_transform ? components::transform() : in_camera.camera_transform;

		typename renderable_type::drawing_input in(output);

		in.camera_transform = camera_transform;
		in.renderable_transform = renderable_transform;
		in.visible_world_area = in_camera.visible_world_area;

		if (renderable_drawing_mode == renderable_drawing_type::NORMAL) {
			renderable.draw(in);
		}
		else if (renderable_drawing_mode == renderable_drawing_type::BORDER_HIGHLIGHTS) {
			if (render.draw_border) {
				static const vec2i offsets[4] = {
					vec2i(-1, 0), vec2i(1, 0), vec2i(0, 1), vec2i(0, -1)
				};

				in.colorize = render.border_color;

				for (const auto o : offsets) {
					in.renderable_transform.pos = renderable_transform.pos + o;
					renderable.draw(in);
				}
			}
		}
		else if (renderable_drawing_mode == renderable_drawing_type::NEON_MAPS) {
			in.use_neon_map = true;
			renderable.draw(in);
		}
	}

	std::array<std::vector<const_entity_handle>, render_layer::COUNT> get_visible_per_layer(const std::vector<const_entity_handle>&) const;
};