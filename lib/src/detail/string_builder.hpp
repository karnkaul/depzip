#pragma once
#include <concepts>
#include <string>

namespace dz::detail {
struct StringBuilder {
	auto append(std::string_view text) -> StringBuilder&;

	template <std::convertible_to<std::string_view>... Ts>
	auto append(Ts const&... ts) -> StringBuilder& {
		(append(std::string_view{ts}), ...);
		return *this;
	}

	std::string value{};
	std::string_view delimiter{" "};
};
} // namespace dz::detail
