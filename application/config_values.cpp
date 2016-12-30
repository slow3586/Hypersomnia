#include "lua.hpp"
#include <luabind/luabind.hpp>

#include "config_values.h"
#include "game_window.h"

#define NVP(x) x, #x

void config_values::get_values(game_window& w) {
	auto set = [&w](auto& c, const std::string& ss) { w.get_config_value(ss, c); };

	set(NVP(launch_mode));
	set(NVP(input_recording_mode));

	set(NVP(recording_replay_speed));

	set(NVP(determinism_test_cloned_cosmoi_count));

	set(NVP(sound_effects_volume));
	set(NVP(music_volume));

	set(NVP(debug_disable_cursor_clipping));

	set(NVP(mouse_sensitivity));

	set(NVP(connect_address));
	set(NVP(connect_port));
	set(NVP(server_port));
	set(NVP(alternative_port));

	set(NVP(nickname));
	set(NVP(debug_second_nickname));

	set(NVP(tickrate));

	set(NVP(jitter_buffer_ms));
	set(NVP(client_commands_jitter_buffer_ms));

	set(NVP(interpolation_speed));
	set(NVP(misprediction_smoothing_multiplier));
	
	set(NVP(debug_var));
	set(NVP(debug_randomize_entropies_in_client_setup));
	set(NVP(debug_randomize_entropies_in_client_setup_once_every_steps));

	set(NVP(server_launch_http_daemon));
	set(NVP(server_http_daemon_port));
	set(NVP(server_http_daemon_html_file_path));

	set(NVP(db_path));
	set(NVP(survey_num_file_path));
	set(NVP(post_data_file_path));
	set(NVP(last_session_update_link));

	set(NVP(director_scenario_filename));
	set(NVP(menu_intro_scenario_filename));

	set(NVP(menu_theme_filename));
	set(NVP(skip_credits));
	set(NVP(latest_news_url));
}
