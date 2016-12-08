#pragma once
#include <array>
#include <unordered_map>

#include "game/components/sound_existence_component.h"
#include "game/transcendental/entity_id.h"
#include "game/transcendental/entity_handle_declaration.h"
#include "game/transcendental/step_declaration.h"

#include "augs/misc/delta.h"

#include "augs/audio/sound_source.h"
struct camera_cone;

class viewing_step;
class interpolation_system;

class sound_system {
public:
	struct cache {
		components::sound_existence recorded_component;
		bool constructed = false;
		augs::sound_source source;
	};

	std::unordered_map<entity_id, cache> per_entity_cache;

	cache& get_cache(const const_entity_handle);
	const cache& get_cache(const const_entity_handle) const;

	std::vector<augs::sound_source> sources;

	void initialize_sound_sources(const size_t num_max_sources);

	void reserve_caches_for_entities(const size_t) const {}

	void play_nearby_sound_existences(
		camera_cone,
		const entity_id listening_character,
		const cosmos&, 
		const float global_time_seconds,
		interpolation_system& sys
		);

	void resample_state_for_audiovisuals(const cosmos&);
};