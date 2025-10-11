#pragma once
#include <string_view>
#include <vector>

namespace dz {
/// \brief Package description.
struct PackageInfo {
	/// \brief Location / identifier of a specific repository
	/// Values of the form 'a/b' get appended to the default host,
	/// else used as is.
	///
	/// Required parameter.
	std::string_view uri;

	/// \brief Git commit-ish to checkout.
	std::string_view branch{};
	/// \brief Subdirectory to clone to, can be empty.
	std::string_view subdir{};
	/// \brief Sub-paths to remove after cloning (eg docs, examples).
	std::vector<std::string_view> remove_subpaths{};
};
} // namespace dz
