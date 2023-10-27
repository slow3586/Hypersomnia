#pragma once

enum class work_result {
	STEAM_RESTART,

	SUCCESS,
	FAILURE,
	RELAUNCH_DEDICATED_SERVER,
	RELAUNCH_AND_UPDATE_DEDICATED_SERVER,
	RELAUNCH_UPGRADED,

	REPORT_UPDATE_AVAILABLE,
	REPORT_UPDATE_UNAVAILABLE
};
