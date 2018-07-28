#pragma once
#include "game/cosmos/cosmos_solvable.h"
#include "game/cosmos/entity_type_traits.h"

template <class... MustHaveComponents, class S, class F>
void cosmos_solvable::for_each_entity_impl(S& self, F callback) {
	self.significant.for_each_entity_pool(
		[&](auto& p) {
			using P = decltype(p);
			using pool_type = remove_cref<P>;

			using Solvable = typename pool_type::mapped_type;
			using E = entity_type_of<Solvable>;

			if constexpr(has_all_of_v<E, MustHaveComponents...>) {
				using index_type = typename pool_type::used_size_type;

				for (index_type i = 0; i < p.size(); ++i) {
					using R = decltype(callback(p.data()[i], i));
					
					if constexpr(std::is_same_v<R, void>) {
						callback(p.data()[i], i);
					}
					else {
						const auto result = callback(p.data()[i], i);

						if constexpr(std::is_same_v<R, callback_result>) {
							if (result == callback_result::ABORT) {
								break;
							}
						}
						else {
							static_assert(always_false_v<E>, "Wrong return type from a callback to for_each_entity.");
						}
					}
				}
			}
		}
	);
}

template <class... MustHaveComponents, class F>
void cosmos_solvable::for_each_entity(F&& callback) {
	for_each_entity_impl<MustHaveComponents...>(*this, std::forward<F>(callback));
}

template <class... MustHaveComponents, class F>
void cosmos_solvable::for_each_entity(F&& callback) const {
	for_each_entity_impl<MustHaveComponents...>(*this, std::forward<F>(callback));
}
