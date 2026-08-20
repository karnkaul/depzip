#pragma once
#include "depzip/manifest.hpp"
#include "djson/json.hpp"

namespace depzip {
/// \brief json must outlive all string views in package.
void from_json(dj::Json const& json, PackageInfo& package);
void to_json(dj::Json& json, PackageInfo const& package);

/// \brief json must outlive all string views in manifest.
void from_json(dj::Json const& json, Manifest& manifest);
void to_json(dj::Json& json, Manifest const& manifest);
} // namespace depzip
