#pragma once
#include <detail/program.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Zip : public Program {
  public:
	static constexpr std::string_view name_v{"zip"};

	explicit Zip() : Program(std::string{name_v}) {}

	void create_archive(std::string_view const dir_to_add) const {
		auto const zip_name = std::format("{}.zip", dir_to_add);
		if (fs::exists(zip_name)) { util::rm_rf(verbosity, zip_name); }
		auto const args = std::format("-r {} {}", zip_name, dir_to_add);
		if (!execute(args)) { throw Panic{std::format("Failed to create ZIP {}", zip_name)}; }
	}
};
} // namespace dz::detail
