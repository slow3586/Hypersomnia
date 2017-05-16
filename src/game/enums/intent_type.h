#pragma once
#include <vector>
#include "augs/misc/container_with_small_size.h"
#include "augs/misc/basic_game_intent.h"

enum class intent_type : unsigned char {
	OPEN_DEVELOPER_CONSOLE,

	THROW,
	// contextual
	THROW_PRIMARY_ITEM,
	THROW_SECONDARY_ITEM,

	HOLSTER,
	// contextual
	HOLSTER_PRIMARY_ITEM,
	HOLSTER_SECONDARY_ITEM,

	START_PICKING_UP_ITEMS,

	USE_BUTTON,
	// contextual
	QUERY_TOUCHING_TRIGGERS,
	RELEASE_CAR,

	SPACE_BUTTON,
	// contextual
	HAND_BRAKE,

	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_LEFT,
	MOVE_RIGHT,
	WALK,
	SPRINT,

	MOVE_CROSSHAIR,
	CROSSHAIR_PRIMARY_ACTION,
	CROSSHAIR_SECONDARY_ACTION,

	PRESS_GUN_TRIGGER,

	RELOAD,

	SWITCH_LOOK,
	SWITCH_TO_GUI,
	DEBUG_SWITCH_CHARACTER,
	SWITCH_WEAPON_LASER,

	MELEE_PRIMARY_MOVE,
	MELEE_SECONDARY_MOVE,
	MELEE_TERTIARY_MOVE,

	HOTBAR_BUTTON_0,
	HOTBAR_BUTTON_1,
	HOTBAR_BUTTON_2,
	HOTBAR_BUTTON_3,
	HOTBAR_BUTTON_4,
	HOTBAR_BUTTON_5,
	HOTBAR_BUTTON_6,
	HOTBAR_BUTTON_7,
	HOTBAR_BUTTON_8,
	HOTBAR_BUTTON_9,

	PREVIOUS_HOTBAR_SELECTION_SETUP,

	SPECIAL_ACTION_BUTTON_1,
	SPECIAL_ACTION_BUTTON_2,
	SPECIAL_ACTION_BUTTON_3,
	SPECIAL_ACTION_BUTTON_4,
	SPECIAL_ACTION_BUTTON_5,
	SPECIAL_ACTION_BUTTON_6,
	SPECIAL_ACTION_BUTTON_7,
	SPECIAL_ACTION_BUTTON_8,
	SPECIAL_ACTION_BUTTON_9,
	SPECIAL_ACTION_BUTTON_10,
	SPECIAL_ACTION_BUTTON_11,
	SPECIAL_ACTION_BUTTON_12,
	
	CLEAR_DEBUG_LINES,

	COUNT
};

template <class intent_enum_type>
struct basic_input_context;

typedef basic_input_context<intent_type> input_context;

typedef basic_game_intent<intent_type> game_intent;

typedef augs::container_with_small_size<
	std::vector<game_intent>,
	unsigned char
> game_intent_vector;