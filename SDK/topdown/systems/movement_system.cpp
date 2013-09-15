#include "movement_system.h"
#include "entity_system/world.h"
#include "../messages/intent_message.h"
#include "../messages/animate_message.h"

using namespace messages;

void movement_system::process_entities(world& owner) {
	auto events = owner.get_message_queue<messages::intent_message>();

	for (auto it : events) {
		switch (it.type) {
		case intent_message::intent::MOVE_FORWARD:
			it.subject->get<components::movement>().moving_forward = it.state_flag;
			break;
		case intent_message::intent::MOVE_BACKWARD:
			it.subject->get<components::movement>().moving_backward = it.state_flag;
			break;
		case intent_message::intent::MOVE_LEFT:
			it.subject->get<components::movement>().moving_left = it.state_flag;
			break;
		case intent_message::intent::MOVE_RIGHT:
			it.subject->get<components::movement>().moving_right = it.state_flag;
			break;
		default: break;
		}
	}

	for (auto it : targets) {
		auto& physics = it->get<components::physics>();
		auto& movement = it->get<components::movement>();

		vec2<> resultant;
		resultant.x = movement.moving_right * movement.acceleration.x - movement.moving_left * movement.acceleration.x;
		resultant.y = movement.moving_backward * movement.acceleration.y - movement.moving_forward * movement.acceleration.y;

		if (resultant.non_zero())
			physics.body->ApplyForce(resultant * PIXELS_TO_METERSf * physics.body->GetMass(), physics.body->GetWorldCenter());

		b2Vec2 vel = physics.body->GetLinearVelocity();
		float32 speed = vel.Normalize() * METERS_TO_PIXELSf;

		if (speed > movement.max_speed)
			physics.body->SetLinearVelocity(movement.max_speed * PIXELS_TO_METERSf * vel);

		animate_message msg;
		msg.animation_type = animate_message::animation::MOVE;
		
		msg.change_speed = true;
		msg.speed_factor = speed / movement.max_speed;
		msg.change_animation = true;
		msg.preserve_state_if_animation_changes = false;
		msg.message_type = ((speed <= 1.f) ? animate_message::type::STOP : animate_message::type::CONTINUE);
		msg.animation_priority = 0;

		for (auto receiver : movement.animation_receivers) {
			animate_message copy(msg);
			copy.subject = receiver.target;
			
			if (!receiver.stop_at_zero_movement) 
				copy.message_type = animate_message::type::CONTINUE;
			
			owner.post_message(copy);
		}
	}
}