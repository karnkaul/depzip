#pragma once
#include <detail/log.hpp>
#include <detail/panic.hpp>
#include <filesystem>

namespace dz::detail {
namespace fs = std::filesystem;

namespace util {
inline void mkdir(Verbosity const verbosity, fs::path const& path) {
	if (path == ".") { return; }
	log_verbose(verbosity, "-- Creating directory {}", path.generic_string());
	auto err = std::error_code{};
	if (!fs::create_directories(path, err) || err != std::error_code{}) { throw Panic{std::format("Failed to create directory {}", path.generic_string())}; }
}

inline void cd(Verbosity const verbosity, fs::path const& path) {
	if (path.empty() || path == ".") { return; }
	log_verbose(verbosity, "-- Changing pwd to {}", path.generic_string());
	auto err = std::error_code{};
	if (fs::current_path(path, err); err != std::error_code{}) { throw Panic{std::format("Failed to change pwd to {}", path.generic_string())}; }
}

inline void rm_rf(Verbosity const verbosity, fs::path const& path) {
	if (!fs::exists(path)) { return; }
	log_verbose(verbosity, "-- Deleting {}", path.generic_string());
	auto err = std::error_code{};
	if (fs::remove_all(path, err) == 0 || err != std::error_code{}) { throw Panic{std::format("Failed to delete {}", path.generic_string())}; }
}
} // namespace util
} // namespace dz::detail
