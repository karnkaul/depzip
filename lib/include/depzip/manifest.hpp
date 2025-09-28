#pragma once
#include <depzip/package_info.hpp>
#include <vector>

namespace dz {
struct Manifest {
	std::vector<PackageInfo> packages{};
};
} // namespace dz
