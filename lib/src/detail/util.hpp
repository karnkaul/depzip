#pragma once
#include <depzip/panic.hpp>
#include <detail/logger.hpp>
#include <filesystem>

namespace dz::detail {
namespace fs = std::filesystem;

struct Util {
	void mkdir(fs::path const& path) const {
		if (path == ".") { return; }
		logger("-- Creating directory {}", path.generic_string());
		if (!fs::create_directories(path)) { throw Panic{std::format("Failed to create directory {}", path.generic_string())}; }
	}

	void cd(fs::path const& path) const {
		if (path.empty() || path == ".") { return; }
		logger("-- Changing pwd to {}", path.generic_string());
		fs::current_path(path);
	}

	void rm_rf(fs::path const& path) const {
		if (!fs::exists(path)) { return; }
		logger("-- Deleting {}", path.generic_string());
		if (fs::remove_all(path) == 0) { throw Panic{std::format("Failed to delete {}", path.generic_string())}; }
	}

	Logger const& logger;
};
} // namespace dz::detail
