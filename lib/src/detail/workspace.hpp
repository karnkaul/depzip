#pragma once
#include <detail/util.hpp>

namespace dz::detail {
class Workspace {
  public:
	explicit Workspace(Util const& util) : util(util) {}

	void setup(fs::path const& working_dir, fs::path src_dir) {
		m_src_dir = std::move(src_dir);
		if (!working_dir.empty() && !fs::exists(working_dir)) { util.mkdir(working_dir); }
		util.cd(working_dir);
		if (!fs::is_directory(m_src_dir)) { util.mkdir(m_src_dir); }
	}

	[[nodiscard]] auto get_src_dir() const -> fs::path const& { return m_src_dir; }

	Util const& util;

  private:
	fs::path m_src_dir{};
};
} // namespace dz::detail
