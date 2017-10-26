#pragma once
#include "augs/readwrite/streams.h"
#include "game/transcendental/cosmic_entropy.h"
#include "augs/readwrite/byte_readwrite_declaration.h"

struct step_packaged_for_network {
	enum class type : unsigned char {
		INVALID,
		NEW_ENTROPY,
		NEW_ENTROPY_WITH_HEARTBEAT
	} step_type = type::INVALID;

	bool shall_reinfer = false;
	bool next_client_commands_accepted = false;
	augs::stream delta;
	guid_mapped_entropy entropy;
};

#if READWRITE_OVERLOAD_TRAITS_INCLUDED
#error "I/O traits were included BEFORE I/O overloads, which may cause them to be omitted under some compilers."
#endif

namespace augs {
	template<class A>
	void read_object_bytes(A& ar, step_packaged_for_network& storage) {
		augs::read_bytes(ar, storage.step_type);

		if (storage.step_type == step_packaged_for_network::type::NEW_ENTROPY) {
			augs::read_bytes(ar, storage.shall_reinfer);
			augs::read_bytes(ar, storage.next_client_commands_accepted);
			augs::read_bytes(ar, storage.entropy);
		}
		else if (storage.step_type == step_packaged_for_network::type::NEW_ENTROPY_WITH_HEARTBEAT) {
			augs::read_bytes(ar, storage.next_client_commands_accepted);
			augs::read_bytes(ar, storage.entropy);

			augs::read_stream_with_size(ar, storage.delta);
		}
		else {
			ensure(false);
		}
	}

	template<class A>
	void write_object_bytes(A& ar, const step_packaged_for_network& written) {
		write(ar, written.step_type);

		if (written.step_type == step_packaged_for_network::type::NEW_ENTROPY) {
			write(ar, written.shall_reinfer);
			write(ar, written.next_client_commands_accepted);
			write(ar, written.entropy);

		}
		else if (written.step_type == step_packaged_for_network::type::NEW_ENTROPY_WITH_HEARTBEAT) {
			write(ar, written.next_client_commands_accepted);
			write(ar, written.entropy);

			write_stream_with_size(ar, written.delta);
		}
		else {
			ensure(false);
		}
	}
}