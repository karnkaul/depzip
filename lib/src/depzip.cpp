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

	explicit Instance(Info const& info) : m_workspace(info.verbosity, info.working_dir, info.source_dir) { m_git.verbosity = m_zip.verbosity = info.verbosity; }

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

		util::rm_rf(m_git.verbosity, clone_params.dest_dir / ".git");
		for (auto const& subpath : info.remove_subpaths) {
			auto const path = clone_params.dest_dir / subpath;
			util::rm_rf(m_git.verbosity, path);
		}
	}

	void create_zip() final {
		m_zip.create_archive(m_workspace.get_src_dir().generic_string());
		log_verbose(m_git.verbosity, "-- Operation complete");
	}

	Workspace m_workspace;

	Git m_git{};
	Zip m_zip{};
};
} // namespace
} // namespace dz::detail

auto dz::create_instance(InstanceInfo const& info) -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(info); }
