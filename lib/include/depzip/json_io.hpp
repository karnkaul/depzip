#pragma once
#include <depzip/manifest.hpp>
#include <depzip/verbosity.hpp>
#include <djson/json.hpp>

namespace dz {
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

/// \brief json must outlive all string views in manifest.
void from_json(dj::Json const& json, Manifest& manifest);
void to_json(dj::Json& json, Manifest const& manifest);
} // namespace dz
