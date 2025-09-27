#pragma once
#include <detail/shell.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Program : public Shell {
  public:
	static constexpr std::string_view does_exist_args_v = "--version";

	explicit Program(Util const& util, std::string command, std::string_view const does_exist_args = does_exist_args_v)
		: Shell(std::move(command)), util(util) {
		if (!Shell::execute(does_exist_args, Verbosity::Silent)) { throw Panic{std::format("{} not found", get_command())}; }
	}

	[[nodiscard]] auto execute(std::string_view const args) const -> Result { return Shell::execute(args, util.logger.verbosity); }

	Util const& util;
};
} // namespace dz::detail
