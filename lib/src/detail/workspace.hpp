#pragma once
#include <filesystem>

namespace depzip::detail {
namespace fs = std::filesystem;

class Workspace {
  public:
	void setup(fs::path const& working_dir, fs::path src_dir, bool wipe_src);

	[[nodiscard]] auto get_src_dir() const -> fs::path const& { return m_src_dir; }

  private:
	fs::path m_src_dir{};
};
} // namespace depzip::detail
