#pragma once
#include <optional>
#include "augs/audio/sound_source.h"
#include "application/setups/draw_setup_gui_input.h"
#include "augs/window_framework/event.h"
#include "view/mode_gui/draw_mode_gui_input.h"

#include "view/mode_gui/arena/arena_scoreboard_gui.h"
#include "view/mode_gui/arena/arena_choose_team_gui.h"
#include "view/mode_gui/arena/arena_buy_menu_gui.h"

struct arena_gui_state {
	arena_scoreboard_gui scoreboard;
	arena_choose_team_gui choose_team;
	arena_buy_menu_gui buy_menu;

	mutable augs::sound_source tick_sound;
	mutable std::optional<float> last_seconds_value;

	bool control(app_ingame_intent_input);

	template <class M>
	void perform_imgui(
		draw_mode_gui_input, 
		const M& mode, 
		const typename M::input&
	);

	template <class M>
	void draw_mode_gui(
		const draw_setup_gui_input&,
		const draw_mode_gui_input&,

		const M& mode, 
		const typename M::input&
	) const;
};
