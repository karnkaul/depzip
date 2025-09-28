#include <depzip/instance.hpp>
#include <depzip/json_io.hpp>
#include <detail/package.hpp>
#include <detail/programs/git.hpp>
#include <detail/programs/zip.hpp>
#include <detail/workspace.hpp>

namespace dz::detail {
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
} // namespace dz::detail

auto dz::create_instance() -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(); }

void dz::from_json(dj::Json const& json, PackageInfo& package) {
	if (json.is_string()) {
		from_json(json, package.repo_uri);
	} else {
		from_json(json["repo_uri"], package.repo_uri);
		from_json(json["repo_provider"], package.repo_provider, package.repo_provider);
		from_json(json["subdir_name"], package.subdir_name, package.subdir_name);
		for (auto const& subpath : json["remove_subpaths"].as_array()) { from_json(subpath, package.remove_subpaths.emplace_back()); }
	}
}

void dz::to_json(dj::Json& json, PackageInfo const& package) {
	to_json(json["repo_uri"], package.repo_uri);
	to_json(json["repo_provider"], package.repo_provider);
	to_json(json["subdir_name"], package.subdir_name);
	for (auto const subpath : package.remove_subpaths) { to_json(json["remove_subpaths"].push_back(), subpath); }
}

void dz::from_json(dj::Json const& json, Manifest& manifest) {
	for (auto const& package : json["packages"].as_array()) { from_json(package, manifest.packages.emplace_back()); }
}

void dz::to_json(dj::Json& json, Manifest const& manifest) {
	for (auto const& package : manifest.packages) { to_json(json["packages"].push_back(), package); }
}
