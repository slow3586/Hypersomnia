#pragma once
#include "augs/math/vec2.h"

namespace assets {
	enum class necessary_image_id {
		// GEN INTROSPECTOR enum class assets::necessary_image_id
		INVALID,
	
		BLANK,
	
		DROP_HAND_ICON,
	
		GUI_CURSOR,
		GUI_CURSOR_RESIZE_NWSE,
		GUI_CURSOR_HOVER,
		GUI_CURSOR_TEXT_INPUT,
		GUI_CURSOR_ADD,
		GUI_CURSOR_MINUS,
		GUI_CURSOR_ERROR,
	
		MENU_GAME_LOGO,
	
		MENU_BUTTON,
	
		MENU_BUTTON_LT,
		MENU_BUTTON_RT,
		MENU_BUTTON_RB,
		MENU_BUTTON_LB,
	
		MENU_BUTTON_L,
		MENU_BUTTON_T,
		MENU_BUTTON_R,
		MENU_BUTTON_B,
	
		MENU_BUTTON_LB_COMPLEMENT,
	
		MENU_BUTTON_LT_BORDER,
		MENU_BUTTON_RT_BORDER,
		MENU_BUTTON_RB_BORDER,
		MENU_BUTTON_LB_BORDER,
	
		MENU_BUTTON_L_BORDER,
		MENU_BUTTON_T_BORDER,
		MENU_BUTTON_R_BORDER,
		MENU_BUTTON_B_BORDER,
	
		MENU_BUTTON_LB_COMPLEMENT_BORDER,
	
		MENU_BUTTON_LT_INTERNAL_BORDER,
		MENU_BUTTON_RT_INTERNAL_BORDER,
		MENU_BUTTON_RB_INTERNAL_BORDER,
		MENU_BUTTON_LB_INTERNAL_BORDER,
	
		HOTBAR_BUTTON,
	
		HOTBAR_BUTTON_LT,
		HOTBAR_BUTTON_RT,
		HOTBAR_BUTTON_RB,
		HOTBAR_BUTTON_LB,
	
		HOTBAR_BUTTON_L,
		HOTBAR_BUTTON_T,
		HOTBAR_BUTTON_R,
		HOTBAR_BUTTON_B,
	
		HOTBAR_BUTTON_LB_COMPLEMENT,
	
		HOTBAR_BUTTON_LT_BORDER,
		HOTBAR_BUTTON_RT_BORDER,
		HOTBAR_BUTTON_RB_BORDER,
		HOTBAR_BUTTON_LB_BORDER,
	
		HOTBAR_BUTTON_L_BORDER,
		HOTBAR_BUTTON_T_BORDER,
		HOTBAR_BUTTON_R_BORDER,
		HOTBAR_BUTTON_B_BORDER,
	
		HOTBAR_BUTTON_LB_COMPLEMENT_BORDER,
	
		HOTBAR_BUTTON_LT_INTERNAL_BORDER,
		HOTBAR_BUTTON_RT_INTERNAL_BORDER,
		HOTBAR_BUTTON_RB_INTERNAL_BORDER,
		HOTBAR_BUTTON_LB_INTERNAL_BORDER,
	
		ACTION_BUTTON_FILLED,
		ACTION_BUTTON_BORDER,
	
		LASER,
		LASER_GLOW,
		LASER_GLOW_EDGE,
	
		CIRCULAR_BAR_MEDIUM,
		CIRCULAR_BAR_SMALL,
	
		CONTAINER_OPEN_ICON,
		CONTAINER_CLOSED_ICON,
	
		ATTACHMENT_CIRCLE_FILLED,
		ATTACHMENT_CIRCLE_BORDER,
		PRIMARY_HAND_ICON,
		SECONDARY_HAND_ICON,
		SHOULDER_SLOT_ICON,
		ARMOR_SLOT_ICON,
		CHAMBER_SLOT_ICON,
		DETACHABLE_MAGAZINE_SLOT_ICON,
		GUN_MUZZLE_SLOT_ICON,
	
		BLINK_1,
		BLINK_2,
		BLINK_3,
		BLINK_4,
		BLINK_5,
		BLINK_6,
		BLINK_7,
	
		WANDERING_CROSS,
		SPELL_BORDER,
	
		CAST_HIGHLIGHT,
	
		COUNT
		// END GEN INTROSPECTOR
	};
	
	inline auto get_cursor_offset(const necessary_image_id id) {
		auto output = vec2i{ 0, 0 };
	
		switch (id) {
		case necessary_image_id::GUI_CURSOR_HOVER: output = { -5, 0 }; break;
		case necessary_image_id::GUI_CURSOR_RESIZE_NWSE: output = { -9, -9 }; break;
		case necessary_image_id::GUI_CURSOR_TEXT_INPUT: output = { -4, -8 }; break;
		default: break;
		}
	
		return output;
	}
}