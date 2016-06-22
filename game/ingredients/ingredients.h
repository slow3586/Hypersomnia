#pragma once
#include "game/entity_handle.h"
#include "math/vec2.h"

#include "game/assets/texture_id.h"
#include "game/components/render_component.h"

#include "graphics/pixel.h"
#include "game/entity_handle.h"

class entity_handle;
class cosmos;

namespace components {
	struct item;
	struct physics_definition;
	struct sprite;
}

namespace ingredients {
	void camera(entity_handle, int w, int h);

	components::item& make_item(entity_handle);
	
	components::sprite& sprite(entity_handle, vec2 pos, assets::texture_id = assets::texture_id::BLANK, augs::rgba col = augs::rgba(255, 255, 255, 255), render_layer = render_layer::GROUND);
	components::sprite& sprite_scalled(entity_handle, vec2 pos, vec2i size, assets::texture_id = assets::texture_id::BLANK, augs::rgba col = augs::rgba(255, 255, 255, 255), render_layer = render_layer::GROUND);
	
	void bullet_round_physics(entity_handle);
	void see_through_dynamic_body(entity_handle);
	void standard_dynamic_body(entity_handle);
	void standard_static_body(entity_handle);

	void wsad_character_physics(entity_handle);
	void wsad_character_legs(entity_handle legs, entity_handle player);
	void wsad_character(entity_handle, entity_handle crosshair_entity);
	void wsad_character_corpse(entity_handle);

	void inject_window_input_to_character(entity_handle target_character, entity_handle camera_entity);

	void make_always_visible(entity_handle);
	void cancel_always_visible(entity_handle);

	void character_inventory(entity_handle);
	void backpack(entity_handle);

	void default_gun_container(entity_handle);
	void default_gun(entity_handle);

	void standard_pathfinding_capability(entity_handle);
	void soldier_intelligence(entity_handle);
}

namespace prefabs {
	entity_handle create_car(cosmos&, components::transform);
	entity_handle create_motorcycle(cosmos&, components::transform);

	entity_handle create_sample_magazine(cosmos&, vec2 pos, std::string space = "0.30", entity_handle charge_inside = entity_handle());
	entity_handle create_sample_suppressor(cosmos& world, vec2 pos);
	entity_handle create_sample_rifle(cosmos&, vec2 pos, entity_handle load_mag = entity_handle());
	entity_handle create_pistol(cosmos&, vec2 pos, entity_handle load_mag = entity_handle());
	entity_handle create_submachine(cosmos&, vec2 pos, entity_handle load_mag = entity_handle());
	
	entity_handle create_pink_charge(cosmos&, vec2 pos, int charges = 23);
	entity_handle create_cyan_charge(cosmos&, vec2 pos, int charges = 30);
	entity_handle create_green_charge(cosmos& world, vec2 pos, int charges = 23);

	entity_handle create_sample_backpack(cosmos&, vec2 pos);

	entity_handle create_character_crosshair(cosmos&);
	entity_handle create_character(cosmos&, vec2 pos);
	entity_handle create_crate(cosmos&, vec2 pos, vec2 size);

	entity_handle create_cyan_urban_machete(cosmos&, vec2 pos);
}