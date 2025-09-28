#include <depzip/instance.hpp>
#include <detail/package.hpp>
#include <detail/programs/git.hpp>
#include <detail/programs/zip.hpp>
#include <detail/workspace.hpp>

namespace dz::detail {
namespace {
class Instance : public dz::Instance {
	void vendor(std::span<PackageInfo const> packages, Config const& config) final {
		setup(config);
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
		auto const zip_name = m_zip.create_archive(m_workspace.get_src_dir().generic_string());
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
