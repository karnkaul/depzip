#pragma once
#include <depzip/package_info.hpp>
#include <filesystem>

namespace dz::detail {
class Git;

namespace fs = std::filesystem;

class Package {
  public:
	using Info = PackageInfo;

	explicit Package(Git const& git, fs::path const& src_dir, Info const& info);

	[[nodiscard]] auto get_subdir() const -> fs::path const& { return m_subdir; }

  private:
	fs::path m_subdir{};
};
} // namespace dz::detail
