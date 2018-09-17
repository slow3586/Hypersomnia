#pragma once
#include "game/detail/entity_handle_mixins/inventory_mixin.h"
#include "game/detail/inventory/direct_attachment_offset.h"
#include "augs/templates/continue_or_callback_result.h"
#include "augs/templates/traits/is_nullopt.h"

template <class E>
template <class S, class I>
callback_result inventory_mixin<E>::for_each_contained_slot_and_item_recursive(
	S slot_callback, 
	I item_callback,
	const optional_slot_flags& filter
) const {
	const auto this_container = *static_cast<const E*>(this);
	auto& cosm = this_container.get_cosmos();

	return this_container.template dispatch_on_having_all_ret<invariants::container>([&](const auto typed_container) {
		if constexpr(std::is_same_v<const std::nullopt_t, decltype(typed_container)>) {
			return callback_result::CONTINUE;
		}
		else {
			auto& container = typed_container.template get<invariants::container>();

			for (const auto& s : container.slots) {
				if (filter && !filter->test(s.first)) {
					continue;
				}

				const auto this_slot_id = inventory_slot_id(s.first, typed_container.get_id());
				const auto slot_callback_result = continue_or_recursive_callback_result(slot_callback, cosm[this_slot_id]);

				if (slot_callback_result == recursive_callback_result::ABORT) {
					return callback_result::ABORT;
				}
				else if (slot_callback_result == recursive_callback_result::CONTINUE_DONT_RECURSE) {
					continue;
				}
				else if (slot_callback_result == recursive_callback_result::CONTINUE_AND_RECURSE) {
					for (const auto& id : get_items_inside(typed_container, s.first)) {
						const auto result = cosm[id].template dispatch_on_having_all_ret<components::item>(
							[&](const auto& child_item_handle) {
								if constexpr(!is_nullopt_v<decltype(child_item_handle)>) {
									const auto r = continue_or_recursive_callback_result(item_callback, child_item_handle);

									if (r == recursive_callback_result::CONTINUE_AND_RECURSE) {
										const auto next_r = child_item_handle.for_each_contained_slot_and_item_recursive(slot_callback, item_callback, filter);

										if (callback_result::ABORT == next_r) {
											return recursive_callback_result::ABORT;
										}
									}

									return r;
								}
								else {
									return recursive_callback_result::ABORT;
								}
							}
						);

						if (result == recursive_callback_result::ABORT) {
							return callback_result::ABORT;
						}
						else if (result == recursive_callback_result::CONTINUE_DONT_RECURSE) {
							continue;
						}
					}
				}
			}

			return callback_result::CONTINUE;
		}
	});
}

template <class E>
template <class A, class G>
void inventory_mixin<E>::for_each_attachment_recursive(
	A attachment_callback,
	G get_offsets_by_torso,
	const attachment_offset_settings& settings,
	const bool flip_hands_order
) const {
	struct node {
		inventory_slot_id parent;
		entity_id child;
		transformr offset;
	};

	thread_local std::vector<node> container_stack;
	container_stack.clear();

	const auto root_container = *static_cast<const E*>(this);
	auto& cosm = root_container.get_cosmos();

	container_stack.push_back({ {}, root_container, transformr() });

	std::size_t stack_i = 0;

	while (stack_i < container_stack.size()) {
		const auto it = container_stack[stack_i++];

		cosm[it.child].template dispatch_on_having_any<invariants::container, components::item>(
			[&](const auto this_attachment) {
				auto current_offset = it.offset;

				if (it.parent.container_entity.is_set()) {
					/* Don't do it for the root */
					const auto direct_offset = direct_attachment_offset(
						cosm[it.parent.container_entity],
						this_attachment,
						get_offsets_by_torso,
						settings,
						it.parent.type
					);

					current_offset *= direct_offset;

					this_attachment.template dispatch_on_having_all<components::item>([&](const auto& typed_attachment) {
						attachment_callback(typed_attachment, current_offset);
					});
				}

				const auto& this_container = this_attachment;

				if (const auto container = this_container.template find<invariants::container>()) {
					for (const auto& s : container->slots) {
						const auto type = s.first;

						if (s.second.makes_physical_connection()) {
							const auto this_container_id = this_container.get_id();

							for (const auto& id : get_items_inside(this_container, type)) {
								auto insert_where = container_stack.end();

								if (flip_hands_order && type == slot_function::SECONDARY_HAND && container_stack.back().parent.type == slot_function::PRIMARY_HAND) {
									insert_where--;
								}

								container_stack.insert(insert_where, { { type, this_container_id }, id, current_offset });
							}
						}
					}
				}
			}
		);
	}
}

template <class E>
template <class I>
void inventory_mixin<E>::for_each_contained_item_recursive(I&& item_callback, const optional_slot_flags& filter) const {
	for_each_contained_slot_and_item_recursive(
		[](auto) {}, 
		std::forward<I>(item_callback),
		filter
	);
}

template <class E>
template <class S>
void inventory_mixin<E>::for_each_contained_slot_recursive(S&& slot_callback, const optional_slot_flags& filter) const {
	for_each_contained_slot_and_item_recursive(
		std::forward<S>(slot_callback), 
		[](auto...) {},
		filter
	);
}

template <class E>
template <class G>
ltrb inventory_mixin<E>::calc_attachments_aabb(G&& get_offsets_by_torso) const {
	ltrb result;

	for_each_attachment_recursive(
		[&result](
			const auto attachment_entity,
			const auto attachment_offset
		) {
			result.contain(attachment_entity.get_aabb(attachment_offset));
		},
		std::forward<G>(get_offsets_by_torso),
		attachment_offset_settings::for_logic()
	);

	return result;
}
