#include <depzip/instance.hpp>
#include <detail/package.hpp>
#include <detail/programs/git.hpp>
#include <detail/programs/zip.hpp>
#include <detail/workspace.hpp>

namespace dz::detail {
namespace {
class Instance : public dz::Instance {
  public:
	using Info = InstanceInfo;

	explicit Instance(Info const& info) : m_logger{.verbosity = info.verbosity} { m_workspace.setup(info.working_dir, info.source_dir); }

  private:
	void add_package(PackageInfo const& info) final {
		auto const& package = m_packages.emplace_back(m_git, m_workspace.get_src_dir(), info);
		m_logger("== Package setup complete: {}\n", package.get_subdir().generic_string());
	}

	void create_zip() final {
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

auto dz::create_instance(InstanceInfo const& info) -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(info); }
