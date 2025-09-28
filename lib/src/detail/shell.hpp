#pragma once
#include <depzip/verbosity.hpp>
#include <detail/string_builder.hpp>
#include <cstdlib>
#include <print>

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

	[[nodiscard]] auto execute(std::string_view const args, Verbosity const verbosity) const -> Result {
		auto const line = StringBuilder{.value = m_command}.append(args, create_suffix(verbosity)).value;
		print_if_verbose(line, verbosity);
		return std::system(line.c_str()); // NOLINT(concurrency-mt-unsafe)
	}

  private:
	[[nodiscard]] static auto create_suffix(Verbosity const verbosity) -> std::string_view {
		if (verbosity != Verbosity::Silent) { return {}; }
#if defined(_WIN32) && !defined(__MINGW__)
		return " >nul 2>nul";
#else
		return "> /dev/null 2>&1";
#endif
	}

	static void print_if_verbose(std::string_view const line, Verbosity const verbosity) {
		if (verbosity != Verbosity::Verbose) { return; }
		std::println("-- {}", line);
	}

	std::string m_command{};
};
} // namespace dz::detail
