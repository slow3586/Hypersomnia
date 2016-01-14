#include "archetypes.h"
#include "entity_system/entity.h"
#include "entity_system/world.h"

#include "game_framework/components/chase_component.h"
#include "game_framework/components/camera_component.h"
#include "game_framework/components/input_component.h"
#include "game_framework/components/crosshair_component.h"
#include "game_framework/components/sprite_component.h"
#include "game_framework/components/movement_component.h"
#include "game_framework/components/lookat_component.h"
#include "game_framework/components/animation_component.h"
#include "game_framework/components/animation_response_component.h"
#include "game_framework/components/physics_component.h"
#include "game_framework/components/children_component.h"
#include "game_framework/components/car_component.h"
#include "game_framework/components/trigger_component.h"

#include "game_framework/game/body_helper.h"

#include "game_framework/globals/filters.h"

namespace prefabs {
	augs::entity_id create_car(augs::world& world, vec2 pos) {
		auto front = world.create_entity();
		auto interior = world.create_entity();
		auto left_wheel = world.create_entity();

		components::children car_children;

		car_children.add_sub_entity(interior);
		car_children.add_sub_entity(left_wheel);
		front->add(car_children);

		{
			components::sprite sprite;
			components::render render;
			components::transform transform;
			components::car car;
			car.left_wheel_trigger = left_wheel;

			transform.pos = pos;

			sprite.set(assets::texture_id::CAR_FRONT, augs::pixel_32(0, 255, 255));
			sprite.size.x = 100;
			sprite.size.y = 50;

			render.layer = components::render::render_layer::DYNAMIC_BODY;

			front->add(sprite);
			front->add(render);
			front->add(transform);
			front->add(car);

			helpers::physics_info info;
			info.from_renderable(front);

			info.filter = filters::dynamic_object();
			info.density = 0.6f;
			//info.angular_damping = 1;
			//info.angled_damping = true;
			//info.linear_damping = 10;

			auto& physics = helpers::create_physics_component(info, front, b2_dynamicBody);
			physics.angular_air_resistance = 3;
			physics.angular_air_resistance = 8;
			//physics.air_resistance = 0.1;
		}



		{
			components::sprite sprite;
			components::render render;
			components::transform transform;
			transform.pos = pos;

			render.layer = components::render::render_layer::CAR_INTERIOR;

			sprite.set(assets::texture_id::CAR_INSIDE, augs::pixel_32(122, 0, 122, 255));
			sprite.size.x = 150;
			sprite.size.y = 250;

			interior->add(sprite);
			interior->add(render);
			interior->add(transform);

			helpers::physics_info info;
			info.from_renderable(interior);
			info.density = 0.6f;
			//info.angular_damping = 1;
			//info.angled_damping = true;
			//info.linear_damping = 10;
			
			info.sensor = true;

			info.filter = filters::dynamic_object();
			auto& physics = helpers::create_physics_component(info, interior, b2_dynamicBody);
			physics.is_friction_ground = true;
			physics.angular_air_resistance = 3;
			physics.angular_air_resistance = 8;

			vec2 offset(0, front->get<components::sprite>().size.y / 2 + sprite.size.y / 2);
			helpers::create_weld_joint(front, interior, offset);
		}


		{
			components::sprite sprite;
			components::render render;
			components::transform transform;
			components::trigger trigger;
			transform.pos = pos;
			trigger.entity_to_be_notified = front;

			render.layer = components::render::render_layer::CAR_INTERIOR;

			sprite.set(assets::texture_id::CAR_INSIDE, augs::pixel_32(255, 0, 0, 255));
			sprite.size.x = 30;
			sprite.size.y = 30;

			left_wheel->add(sprite);
			left_wheel->add(render);
			left_wheel->add(transform);
			left_wheel->add(trigger);

			helpers::physics_info info;
			info.from_renderable(left_wheel);
			info.density = 0.6f;
			info.filter = filters::trigger();
			info.sensor = true;
			
			auto& physics = helpers::create_physics_component(info, left_wheel, b2_dynamicBody);
			physics.angular_air_resistance = 8;

			vec2 offset(0, front->get<components::sprite>().size.y / 2 + sprite.size.y / 2);
			helpers::create_weld_joint(front, left_wheel, offset);
		}

		return front;
	}

}