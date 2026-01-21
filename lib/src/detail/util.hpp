#pragma once
#include <filesystem>

namespace dz::detail {
namespace fs = std::filesystem;

namespace util {
void mkdir(fs::path const& path);
void cd(fs::path const& path);
void rm_rf(fs::path const& path);
} // namespace util
} // namespace dz::detail
