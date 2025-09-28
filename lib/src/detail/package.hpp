#pragma once
#include <depzip/package_info.hpp>
#include <detail/programs/git.hpp>

namespace dz::detail {
class Package {
  public:
	using Info = PackageInfo;

	explicit Package(Git const& git, fs::path const& src_dir, Info const& info) {
		m_subdir = fs::path{info.subdir_name};
		if (m_subdir.empty()) { m_subdir = fs::path{info.repo_uri}.filename(); }

		auto const clone_params = Git::Clone{
			.url = std::format("{}{}", info.repo_provider, info.repo_uri),
			.tag = info.git_tag,
			.dest_dir = src_dir / m_subdir,
		};
		git.clone(clone_params);

		git.util.rm_rf(clone_params.dest_dir / ".git");
		for (auto const& subpath : info.remove_subpaths) {
			auto const path = clone_params.dest_dir / subpath;
			git.util.rm_rf(path);
		}
	}

	[[nodiscard]] auto get_subdir() const -> fs::path const& { return m_subdir; }

  private:
	fs::path m_subdir{};
};
} // namespace dz::detail
