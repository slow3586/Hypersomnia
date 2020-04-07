#pragma once
#include <unordered_map>
#include "application/nat/stun_session.h"
#include "application/nat/stun_counter_type.h"
#include "application/nat/nat_detection_settings.h"
#include "application/nat/nat_traversal_settings.h"
#include "application/masterserver/nat_traversal_step_payload.h"
#include "augs/misc/randomization.h"
#include "application/nat/nat_type.h"

struct server_nat_traversal_input {
	nat_detection_settings detection_settings;
	nat_traversal_settings traversal_settings;

	stun_counter_type& current_stun_index;
};

class server_nat_traversal {
	struct session {
		double session_guid = 0.0;
		net_time_t when_appeared;
		std::optional<stun_session> stun;
		bool holes_opened = false;
		nat_traversal_step_payload last_payload;

		session();

		void open_holes(netcode_address_t, netcode_packet_queue&);
	};

	randomization stun_rng = randomization::from_random_device();
	netcode_packet_queue packet_queue;
	std::unordered_map<netcode_address_t, session> traversals;

	bool handle_stun_response(const std::byte*, std::size_t n);
	void open_holes_for_client(netcode_address_t);
	bool handle_stun_packet(const std::byte* packet_buffer, const std::size_t packet_bytes);

public:
	nat_detection_result last_detected_nat;
	server_nat_traversal_input input;

	server_nat_traversal(const server_nat_traversal_input&);

	void advance();

	bool handle_auxiliary_command(const netcode_address_t&, const std::byte*, std::size_t n);

	void send_packets(netcode_socket_t socket);
	bool is_behind_nat() const;
};
