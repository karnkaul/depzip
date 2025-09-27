#pragma once
#include <detail/panic.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Workspace {
  public:
	explicit Workspace(Verbosity const verbosity, fs::path const& working_dir, fs::path src_dir) : m_src_dir(std::move(src_dir)) {
		if (!working_dir.empty() && !fs::exists(working_dir)) { util::mkdir(verbosity, working_dir); }
		util::cd(verbosity, working_dir);
		if (!fs::is_directory(m_src_dir)) { util::mkdir(verbosity, m_src_dir); }
	}

	[[nodiscard]] auto get_src_dir() const -> fs::path const& { return m_src_dir; }

  private:
	fs::path m_src_dir{};
};
} // namespace dz::detail
