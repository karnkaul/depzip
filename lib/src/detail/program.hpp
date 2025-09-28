#pragma once
#include <detail/shell.hpp>

namespace dz::detail {
struct Util;

class Program : public Shell {
  public:
	static constexpr std::string_view does_exist_args_v = "--version";

	explicit Program(Util const& util, std::string command, std::string_view does_exist_args = does_exist_args_v);

	[[nodiscard]] auto execute(std::string_view args) const -> Result;

	Util const& util;
};
} // namespace dz::detail
