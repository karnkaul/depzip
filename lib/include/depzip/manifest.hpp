#pragma once
#include <depzip/package_info.hpp>
#include <vector>

namespace dz {
/// \brief Manifest description.
struct Manifest {
	static constexpr std::string_view default_host_v{"https://github.com"};

	/// \brief List of package descriptions.
	///
	/// Required parameter.
	std::vector<PackageInfo> packages{};

	/// \brief Default remote git host.
	/// Defaults to default_host_v if empty.
	std::string_view default_host{default_host_v};
};
} // namespace dz
