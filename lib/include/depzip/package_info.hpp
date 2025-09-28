#pragma once
#include <string_view>
#include <vector>

namespace dz {
/// \brief Package description.
struct PackageInfo {
	/// \brief Sub-URI for a specific repository (eg octocat/Hello-World).
	/// Required parameter.
	std::string_view repo_uri;

	/// \brief Repository host URL (eg github.com).
	std::string_view repo_provider{"https://github.com/"};
	/// \brief Git commit-ish to checkout.
	std::string_view git_tag{};
	/// \brief Subdirectory to clone to, can be empty.
	std::string_view subdir_name{};
	/// \brief Sub-paths to remove after cloning (eg docs, examples).
	std::vector<std::string_view> remove_subpaths{};
};
} // namespace dz
