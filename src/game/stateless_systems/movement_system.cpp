#include "augs/math/vec2.h"
#include "movement_system.h"
#include "game/cosmos/cosmos.h"
#include "game/messages/intent_message.h"

#include "game/components/gun_component.h"

#include "game/components/rigid_body_component.h"
#include "game/components/movement_component.h"
#include "game/components/sentience_component.h"
#include "game/components/torso_component.hpp"

#include "game/cosmos/entity_handle.h"
#include "game/cosmos/logic_step.h"
#include "game/cosmos/data_living_one_step.h"
#include "game/cosmos/for_each_entity.h"

#include "game/stateless_systems/sentience_system.h"

#include "game/detail/physics/physics_scripts.h"
#include "game/detail/frame_calculation.h"
#include "game/detail/visible_entities.h"

using namespace augs;

void movement_system::set_movement_flags_from_input(const logic_step step) {
	auto& cosm = step.get_cosmos();
	const auto& events = step.get_queue<messages::intent_message>();

	for (const auto& it : events) {
		cosm(
			it.subject,
			[&](const auto subject) {
				if (auto* const movement = subject.template find<components::movement>()) {
					switch (it.intent) {
						case game_intent_type::MOVE_FORWARD:
							movement->flags.forward = it.was_pressed();
							break;
						case game_intent_type::MOVE_BACKWARD:
							movement->flags.backward = it.was_pressed();
							break;
						case game_intent_type::MOVE_LEFT:
							movement->flags.left = it.was_pressed();
							break;
						case game_intent_type::MOVE_RIGHT:
							movement->flags.right = it.was_pressed();
							break;
						case game_intent_type::WALK:
							movement->flags.walking = it.was_pressed();
							break;
						case game_intent_type::SPRINT:
							movement->flags.sprint = it.was_pressed();
							break;
						case game_intent_type::START_PICKING_UP_ITEMS:
							movement->flags.picking = it.was_pressed();
							break;

						default: break;
					}
				}
			}
		);
	}
}

void movement_system::apply_movement_forces(const logic_step step) {
	auto& cosm = step.get_cosmos();

	const auto& clk = cosm.get_clock();
	const auto delta = clk.dt;
	const auto delta_ms = delta.in_milliseconds();

	cosm.for_each_having<components::movement>(
		[&](const auto it) {
			auto& movement = it.template get<components::movement>();
			auto& movement_def = it.template get<invariants::movement>();

			const auto& rigid_body = it.template get<components::rigid_body>();

			if (!rigid_body.is_constructed()) {
				return;
			}

			components::sentience* const sentience = it.template find<components::sentience>();
			const bool is_sentient = sentience != nullptr;

			if (it.sentient_and_unconscious()) {
				/* Behave as if all input was unset */
				movement.reset_movement_flags();
			}

			auto movement_force_mult = 1.f;

			movement.was_sprint_effective = movement.flags.sprint;

			value_meter::damage_result consciousness_damage_by_sprint;

			enum class haste_type {
				NONE,
				NORMAL,
				GREATER
			};

			const auto current_haste = [&]() {
				if (is_sentient) {
					const auto& haste = sentience->get<haste_perk_instance>();

					if (haste.timing.is_enabled(clk)) {
						if (haste.is_greater) {
							return haste_type::GREATER;
						}
						else {
							return haste_type::NORMAL;
						}
					}
				}

				return haste_type::NONE;
			}();

			if (is_sentient) {
				const auto& def = it.template get<invariants::sentience>();

				auto& consciousness = sentience->get<consciousness_meter_instance>();

				const auto minimum_cp_to_sprint = consciousness.get_maximum_value() * def.minimum_cp_to_sprint;

				if (consciousness.value < minimum_cp_to_sprint - 0.1f) {
					movement_force_mult /= 2;
				}

				consciousness_damage_by_sprint = consciousness.calc_damage_result(
					def.sprint_drains_cp_per_second * delta.in_seconds(),
					minimum_cp_to_sprint
				);

				if (consciousness_damage_by_sprint.excessive > 0) {
					movement.was_sprint_effective = false;
				}

				if (current_haste == haste_type::GREATER) {
					movement_force_mult *= 1.45f;
				}
				else if (current_haste == haste_type::NORMAL) {
					movement_force_mult *= 1.3f;
				}
			}

			const bool is_inert = movement.make_inert_for_ms > 0.f;

			if (is_inert) {
				movement.make_inert_for_ms -= static_cast<float>(delta_ms);
			}

			const auto num_frames = movement_def.animation_frame_num;
			const auto frame_ms = movement_def.animation_frame_ms;

			auto calculate_frame_idx = [&]() {
				auto idx = static_cast<unsigned>(movement.animation_amount / frame_ms);
				idx = std::min(idx, num_frames - 1);
				idx = std::max(idx, 0u);

				return idx;
			};

			const bool is_walking = !movement.was_sprint_effective && [&]() {
				if (movement.flags.picking) {
					return true;
				}

				return movement.flags.walking;
			}();

			movement.was_walk_effective = is_walking;

			const bool should_decelerate_due_to_walk = is_walking && movement.animation_amount >= num_frames * frame_ms - 60.f;

			const auto requested_by_input = movement.get_force_requested_by_input(movement_def.input_acceleration_axes);
			const bool propelling = !should_decelerate_due_to_walk && requested_by_input.non_zero();

			if (propelling) {
				if (movement.was_sprint_effective) {
					movement_force_mult /= 2.f;

					if (is_sentient) {
						sentience->get<consciousness_meter_instance>().value -= consciousness_damage_by_sprint.effective;
					}
				}

				if (is_inert) {
					movement_force_mult /= 10.f;
				}

				if (is_sentient) {
					sentience->time_of_last_exertion = cosm.get_timestamp();
				}

				auto applied_force = requested_by_input;

				if (movement_def.acceleration_length > 0) {
					applied_force.set_length(movement_def.acceleration_length);
				}

				applied_force *= movement_force_mult;

				rigid_body.apply_force(
					applied_force, 
					movement_def.applied_force_offset
				);
			}

			const auto duration_bound = static_cast<float>(num_frames * frame_ms);

			const auto conceptual_max_speed = std::max(1.f, movement_def.max_speed_for_animation);
			const auto current_velocity = rigid_body.get_velocity();
			const auto current_speed = current_velocity.length();

			const auto speed_mult = current_speed / conceptual_max_speed;

			auto start_footstep_effect = [&]() {
				const auto transform = it.get_logic_transform();

				const auto velocity_degrees = current_velocity.degrees();
				auto effect_transform = transformr(transform.pos, velocity_degrees);

				const auto leg_anim = it.template get<invariants::torso>().calc_leg_anim(
					current_velocity,
					transform.rotation
				);

				const auto anim_id = leg_anim.id;

				const auto& logicals = cosm.get_logical_assets();

				if (const auto anim = logicals.find(anim_id)) {
					/* Offset the effect transform by leg offset, if the animation exists */
					const auto frame = ::get_frame_and_flip(movement.four_ways_animation, *anim);
					const auto& im_def = logicals.get_offsets(frame.frame.image_id);

					auto offset = im_def.legs.foot;

					if (frame.flip.vertically) {
						offset.y *= -1;
					}

					if (frame.flip.horizontally) {
						offset.x *= -1;
					}

					effect_transform *= transformr(offset);
				}

				const auto& common_assets = cosm.get_common_assets();
				auto chosen_effect = common_assets.standard_footstep;
				
				{
					/* Choose effect based on where the foot has landed */

					const auto ground_id = get_hovered_world_entity(
						cosm,
						effect_transform.pos,
						[&cosm](const auto id) {
							return cosm[id].template has<invariants::ground>();
						},
						render_layer_filter::whitelist(
							render_layer::CAR_INTERIOR,
							render_layer::ON_ON_FLOOR,
							render_layer::ON_FLOOR,
							render_layer::FLOOR_AND_ROAD,
							render_layer::GROUND,
							render_layer::UNDER_GROUND
						)
					);

					if (const auto ground_entity = cosm[ground_id]) {
						const auto& ground = ground_entity.template get<invariants::ground>();

						if (ground.footstep_effect.is_enabled) {
							chosen_effect = ground.footstep_effect.value;
						}
					}
				}

				auto& sound = chosen_effect.sound;
				const auto gain_mult = speed_mult / 2;
				const auto pitch_mult = std::min(1.7f, 1 + gain_mult);

				sound.modifier.gain *= gain_mult;
				sound.modifier.pitch *= pitch_mult;

				sound.start(
					step, 
					sound_effect_start_input::at_entity(it.get_id())
				);

				const auto scale = std::max(0.8f, speed_mult);

				{
					auto& particles = chosen_effect.particles;
					particles.modifier.scale_amounts *= scale;
					particles.modifier.scale_lifetimes *= scale;

					particles.start(
						step, 
						particle_effect_start_input::fire_and_forget(effect_transform)
					);
				}

				if (current_haste != haste_type::NONE) {
					auto particles = common_assets.haste_footstep_particles;

					particles.modifier.scale_amounts *= scale;
					particles.modifier.scale_lifetimes *= scale;
					particles.modifier.colorize = green;

					effect_transform.rotation += 180;

					particles.start(
						step, 
						particle_effect_start_input::fire_and_forget(effect_transform)
					);
				}
			};

			const auto animation_dt = delta_ms * speed_mult;

			auto& backward = movement.four_ways_animation.backward;
			auto& amount = movement.animation_amount;

			if (!propelling && current_speed <= conceptual_max_speed / 2) {
				/* Animation is finishing. */
				const auto decreasing_dt = delta_ms * std::max(sqrt(sqrt(speed_mult)), 0.2f);
				amount = std::max(0.f, amount - decreasing_dt);
			}
			else {
				if (backward) {
					amount -= animation_dt;

					if (augs::flip_if_lt(amount, 0.f)) {
						backward = false;

						auto& f = movement.four_ways_animation.flip;
						f = !f;
					}
				}
				else {
					amount += animation_dt;

					if (augs::flip_if_gt(amount, duration_bound)) {
						backward = true;
					}
				}
			}

			movement.four_ways_animation.base_frames_n = num_frames;

			auto& idx = movement.four_ways_animation.index;

			const auto old_idx = idx;
			idx = calculate_frame_idx();

			if (old_idx == num_frames - 2 && idx == num_frames - 1) {
				start_footstep_effect();
			}

			rigid_body.infer_caches();
		}
	);
}
