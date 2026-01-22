#pragma once
#include <string_view>

namespace depzip::detail {
class Program {
  public:
	static constexpr std::string_view does_exist_args_v = "--version";

	explicit Program(std::string_view command, std::string_view does_exist_args = does_exist_args_v);

	[[nodiscard]] auto get_command() const -> std::string_view { return m_command; }

	[[nodiscard]] auto execute(std::string_view args) const -> bool;

  private:
	std::string_view m_command{};
};
} // namespace depzip::detail
