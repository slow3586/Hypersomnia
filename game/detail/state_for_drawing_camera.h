#pragma once
#include "augs/math/vec2.h"
#include "augs/math/rects.h"
#include "game/components/transform_component.h"
#include "game/transcendental/entity_id.h"

struct state_for_drawing_camera {
	entity_id associated_character;
	components::transform camera_transform;

	vec2 visible_world_area;

	// used for visibility queries
	augs::rects::ltrb<float> transformed_visible_world_area_aabb;

	template <class Archive>
	void serialize(Archive& ar) {
		ar(
			CEREAL_NVP(associated_character),
			CEREAL_NVP(camera_transform),

			CEREAL_NVP(visible_world_area),

			CEREAL_NVP(transformed_visible_world_area_aabb));
	}
};