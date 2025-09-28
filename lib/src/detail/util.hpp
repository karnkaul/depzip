#pragma once
#include <filesystem>

namespace dz::detail {
struct Logger;

namespace fs = std::filesystem;

struct Util {
	void mkdir(fs::path const& path) const;
	void cd(fs::path const& path) const;
	void rm_rf(fs::path const& path) const;

	Logger const& logger;
};
} // namespace dz::detail
