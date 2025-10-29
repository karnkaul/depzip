#pragma once
#include <depzip/verbosity.hpp>
#include <cstdlib>
#include <string_view>

namespace dz::detail::shell {
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

[[nodiscard]] auto execute(Verbosity verbosity, std::string_view command, std::string_view args = {}) -> Result;
} // namespace dz::detail::shell
