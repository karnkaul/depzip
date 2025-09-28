#pragma once
#include <depzip/verbosity.hpp>
#include <cstdlib>
#include <string>

namespace dz::detail {
class Shell {
  public:
	class Result {
	  public:
		explicit(false) constexpr Result(int const code) : m_code(code) {}

		[[nodiscard]] constexpr auto get_code() const -> int { return m_code; }
		[[nodiscard]] constexpr auto is_success() const -> bool { return get_code() == EXIT_SUCCESS; }

		explicit(false) constexpr operator int() const { return get_code(); }
		explicit constexpr operator bool() const { return is_success(); }

	  private:
		int m_code{};
	};

	explicit Shell(std::string command) : m_command(std::move(command)) {}

	[[nodiscard]] auto get_command() const -> std::string_view { return m_command; }

	[[nodiscard]] auto execute(std::string_view args, Verbosity verbosity) const -> Result;

  private:
	[[nodiscard]] static auto create_suffix(Verbosity verbosity) -> std::string_view;

	static void print_if_verbose(std::string_view line, Verbosity verbosity);

	std::string m_command{};
};
} // namespace dz::detail
