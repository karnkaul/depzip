#pragma once
#include "depzip/package_info.hpp"
#include "klib/ptr.hpp"
#include "klib/task/task.hpp"
#include <atomic>
#include <filesystem>

namespace depzip::detail {
class Git;

namespace fs = std::filesystem;

class SetupPackage : public klib::task::Task {
  public:
	enum class State : std::int8_t { Idle, Busy, Complete, Failed };

	using Info = PackageInfo;

	explicit SetupPackage(Git const& git, fs::path const& src_dir, Info const& info);

	[[nodiscard]] auto get_state() const -> State { return m_state; }

	void execute() final;

  private:
	klib::Ptr<Git const> m_git{};
	klib::Ptr<fs::path const> m_src_dir{};
	klib::Ptr<Info const> m_info{};

	std::atomic<State> m_state{State::Idle};
};
} // namespace depzip::detail
