#pragma once
#include "game/entity_handle_declaration.h"

template<bool is_const, class entity_handle_type>
class physics_getters {
public:
	entity_handle_type get_owner_friction_ground() const;
};