#pragma once
#include <string_view>
#include <vector>

namespace dz {
struct PackageInfo {
	std::string_view repo_provider{"https://github.com/"};
	std::string_view repo_uri{};
	std::string_view git_tag{"main"};
	std::string_view subdir_name{};
	std::vector<std::string_view> remove_subpaths{};
};
} // namespace dz
