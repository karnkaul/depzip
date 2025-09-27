#pragma once
#include <depzip/verbosity.hpp>
#include <print>

namespace dz::detail {
template <typename... Args>
void log_verbose(Verbosity const verbosity, std::format_string<Args...> fmt, Args&&... args) {
	if (verbosity != Verbosity::Verbose) { return; }
	std::println(fmt, std::forward<Args>(args)...);
}
} // namespace dz::detail
