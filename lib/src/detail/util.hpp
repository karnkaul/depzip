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

		auto const extract_failed_path = [](std::string_view const fs_err) -> std::string_view {
			auto const lbrace = fs_err.find('[');
			if (lbrace == std::string_view::npos) { return {}; }
			auto ret = fs_err.substr(lbrace + 1);
			auto const rbrace = ret.find(']');
			if (rbrace < ret.size()) { ret.remove_suffix(ret.size() - rbrace); }
			return ret;
		};

		static constexpr auto max_attempts_v{10};
		for (auto attempt = 1; attempt <= max_attempts_v; ++attempt) {
			try {
				fs::remove_all(path);
				return;
			} catch (fs::filesystem_error const& e) {
				auto const failed_path = extract_failed_path(e.what());
				if (failed_path.empty()) { std::rethrow_exception(std::current_exception()); }

				logger("{}\n  changing perms and retrying (attempt = {})...", e.what(), attempt);
				fs::permissions(failed_path, fs::perms::owner_write | fs::perms::group_write);
			}
		}

		throw Panic{std::format("Failed to delete {}", path.generic_string())};
	}

	Logger const& logger;
};
} // namespace dz::detail
