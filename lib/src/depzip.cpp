#include <depzip/instance.hpp>
#include <depzip/package_info.hpp>
#include <detail/git.hpp>
#include <detail/workspace.hpp>
#include <detail/zip.hpp>

namespace dz::detail {
namespace {
class Instance : public dz::Instance {
  public:
	using Info = InstanceInfo;

	explicit Instance(Info const& info) : m_logger{.verbosity = info.verbosity}, m_workspace(m_util, info.working_dir, info.source_dir) {}

  private:
	void add_package(PackageInfo const& info) final {
		auto subdir_name = fs::path{info.subdir_name};
		if (subdir_name.empty()) { subdir_name = fs::path{info.repo_uri}.filename(); }

		auto const clone_params = Git::Clone{
			.url = std::format("{}/{}", info.repo_provider, info.repo_uri),
			.tag = info.git_tag,
			.dest_dir = m_workspace.get_src_dir() / subdir_name,
		};
		m_git.clone(clone_params);

		m_util.rm_rf(clone_params.dest_dir / ".git");
		for (auto const& subpath : info.remove_subpaths) {
			auto const path = clone_params.dest_dir / subpath;
			m_util.rm_rf(path);
		}
	}

	void create_zip() final {
		m_zip.create_archive(m_workspace.get_src_dir().generic_string());
		m_logger("-- Operation complete");
	}

	Logger m_logger{};
	Util m_util{m_logger};
	Workspace m_workspace;

	Git m_git{m_util};
	Zip m_zip{m_util};
};
} // namespace
} // namespace dz::detail

auto dz::create_instance(InstanceInfo const& info) -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(info); }
