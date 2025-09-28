#pragma once
#include <depzip/verbosity.hpp>
#include <print>

namespace dz::detail {
struct Logger {
	template <typename... Args>
	void operator()(std::format_string<Args...> fmt, Args&&... args) const {
		if (verbosity != Verbosity::Verbose) { return; }
		std::println(fmt, std::forward<Args>(args)...);
	}

	Verbosity verbosity{Verbosity::Default};
};
} // namespace dz::detail
