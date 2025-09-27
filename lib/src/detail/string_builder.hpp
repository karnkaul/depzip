#pragma once
#include <concepts>
#include <format>
#include <string>

namespace dz::detail {
struct StringBuilder {
	auto append(std::string_view const text) -> StringBuilder& {
		if (text.empty()) { return *this; }
		if (value.empty()) {
			value = text;
		} else {
			std::format_to(std::back_inserter(value), " {}", text);
		}
		return *this;
	}

	template <std::convertible_to<std::string_view>... Ts>
	auto append(Ts const&... ts) -> StringBuilder& {
		(append(std::string_view{ts}), ...);
		return *this;
	}

	std::string value{};
	std::string_view delimiter{" "};
};
} // namespace dz::detail
