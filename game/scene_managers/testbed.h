#pragma once
#include <vector>
#include "game/transcendental/entity_id.h"
#include "game/transcendental/cosmos.h"
#include "application/game_window.h"
#include "game/transcendental/entity_handle_declaration.h"

#include "augs/misc/constant_size_vector.h"
#include "game/container_sizes.h"

namespace augs {
	struct machine_entropy;
}

struct cosmic_entropy;
class fixed_step;
class cosmos;
class world_camera;
class viewing_session;
struct input_context;

namespace scene_managers {
	class testbed {
		void populate(fixed_step&);
	public:
		augs::constant_size_vector<entity_id, TESTBED_CHARACTERS_COUNT> characters;
		augs::constant_size_vector<entity_id, 100> crates;
		unsigned current_character = 0;
		entity_id currently_controlled_character;
		int debug_var = 0;
		augs::constant_size_vector<entity_id, TESTBED_DRAW_BODIES_COUNT> draw_bodies;

		cosmos stashed_cosmos;
		augs::stream stashed_delta;

		void configure_view(viewing_session&) const;

		void populate_world_with_entities(cosmos&);

		void control(const augs::machine_entropy::local_type&, cosmos&);
		
		cosmic_entropy make_cosmic_entropy(const augs::machine_entropy::local_type&, const input_context&, const cosmos&);
		entity_id get_controlled_entity() const;

		void inject_input_to(entity_handle);

		void step_with_callbacks(const cosmic_entropy&, cosmos&, viewing_session& post_solve_effects_response);

		void pre_solve(fixed_step&);
		void post_solve(fixed_step&);
	};
}