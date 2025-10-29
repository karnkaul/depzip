#pragma once
#include <string_view>

namespace dz::detail {
struct Util;

class Program {
  public:
	static constexpr std::string_view does_exist_args_v = "--version";

	explicit Program(Util const& util, std::string_view command, std::string_view does_exist_args = does_exist_args_v);

	[[nodiscard]] auto get_command() const -> std::string_view { return m_command; }

	[[nodiscard]] auto execute(std::string_view args) const -> bool;

	Util const& util;

  private:
	std::string_view m_command{};
};
} // namespace dz::detail
