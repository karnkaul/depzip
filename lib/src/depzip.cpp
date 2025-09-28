#include <depzip/instance.hpp>
#include <depzip/json_io.hpp>
#include <detail/package.hpp>
#include <detail/programs/git.hpp>
#include <detail/programs/zip.hpp>
#include <detail/workspace.hpp>
#include <chrono>
#include <thread>

namespace dz::detail {
using namespace std::chrono_literals;

namespace {
class Instance : public dz::Instance {
	void vendor(std::span<PackageInfo const> packages, Config const& config) final {
		setup(config);

		if (packages.empty()) {
			std::println("Nothing to vendor");
			return;
		}

		for (auto const& package_info : packages) { add_package(package_info); }
		create_zip();
	}

	void setup(Config const& config) {
		m_logger.verbosity = config.verbosity;
		m_workspace.setup(config.working_dir, config.source_dir);
	}

	void add_package(PackageInfo const& package_info) {
		auto const& package = m_packages.emplace_back(m_git, m_workspace.get_src_dir(), package_info);
		m_logger("== Package setup complete: {}\n", package.get_subdir().generic_string());
	}

	void create_zip() {
		auto const zip_name = m_zip.create_archive(m_workspace.get_src_dir());
		m_logger("== ZIP file {} created\n", zip_name);
	}

	Logger m_logger{};
	Util m_util{m_logger};
	Workspace m_workspace{m_util};

	Git m_git{m_util};
	Zip m_zip{m_util};

	std::vector<Package> m_packages{};
};
} // namespace

void Util::rm_rf(fs::path const& path) const {
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
	auto retry_delay = 5ms;
	for (auto attempt = 1; attempt <= max_attempts_v; ++attempt) {
		try {
			fs::remove_all(path);
			return;
		} catch (fs::filesystem_error const& e) {
			auto const failed_path = extract_failed_path(e.what());
			if (failed_path.empty()) { std::rethrow_exception(std::current_exception()); }

			logger("{}\n  waiting {}ms before changing perms and retrying... (attempt: {})", e.what(), retry_delay.count(), attempt);
			std::this_thread::sleep_for(retry_delay);
			retry_delay *= 2;
			fs::permissions(failed_path, fs::perms::owner_write);
		}
	}

	throw Panic{std::format("Failed to delete {}", path.generic_string())};
}
} // namespace dz::detail

auto dz::create_instance() -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(); }

void dz::from_json(dj::Json const& json, PackageInfo& package) {
	if (json.is_string()) {
		from_json(json, package.repo_uri);
	} else {
		from_json(json["repo_uri"], package.repo_uri);
		from_json(json["repo_provider"], package.repo_provider, package.repo_provider);
		from_json(json["git_tag"], package.git_tag, package.git_tag);
		from_json(json["subdir_name"], package.subdir_name, package.subdir_name);
		for (auto const& subpath : json["remove_subpaths"].as_array()) { from_json(subpath, package.remove_subpaths.emplace_back()); }
	}
}

void dz::to_json(dj::Json& json, PackageInfo const& package) {
	to_json(json["repo_uri"], package.repo_uri);
	to_json(json["repo_provider"], package.repo_provider);
	to_json(json["git_tag"], package.git_tag);
	to_json(json["subdir_name"], package.subdir_name);
	for (auto const subpath : package.remove_subpaths) { to_json(json["remove_subpaths"].push_back(), subpath); }
}

void dz::from_json(dj::Json const& json, Manifest& manifest) {
	for (auto const& package : json["packages"].as_array()) { from_json(package, manifest.packages.emplace_back()); }
}

void dz::to_json(dj::Json& json, Manifest const& manifest) {
	for (auto const& package : manifest.packages) { to_json(json["packages"].push_back(), package); }
}
