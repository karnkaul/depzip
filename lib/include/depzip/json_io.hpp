#pragma once
#include <depzip/config.hpp>
#include <depzip/package_info.hpp>
#include <djson/json.hpp>
#include <vector>

namespace dz {
struct VendorParams {
	std::vector<PackageInfo> packages{};
	Config config{};
};

[[nodiscard]] constexpr auto to_string_view(Verbosity const verbosity) -> std::string_view {
	switch (verbosity) {
	default:
	case Verbosity::Default: return "default";
	case Verbosity::Silent: return "silent";
	case Verbosity::Verbose: return "verbose";
	}
}

[[nodiscard]] constexpr auto to_verbosity(std::string_view const text) -> Verbosity {
	if (text == "verbose") { return Verbosity::Verbose; }
	if (text == "silent") { return Verbosity::Silent; }
	return Verbosity::Default;
}

/// \brief json must outlive all string views in package.
void from_json(dj::Json const& json, PackageInfo& package);
void to_json(dj::Json& json, PackageInfo const& package);

/// \brief json must outlive all string views in config.
void from_json(dj::Json const& json, Config& config);
void to_json(dj::Json& json, Config const& config);

/// \brief json must outlive all string views in params.
void from_json(dj::Json const& json, VendorParams& params);
void to_json(dj::Json& json, VendorParams const& params);
} // namespace dz
