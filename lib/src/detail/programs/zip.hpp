#pragma once
#include <detail/program.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Zip : public Program {
  public:
	static constexpr std::string_view name_v =
#if defined(_WIN32)
		"tar";
#else
		"zip";
#endif

	explicit Zip(Util const& util) : Program(util, std::string{name_v}) {}

	[[nodiscard]] auto create_archive(fs::path const& dir_to_add) const -> std::string {
		auto const zip_name = std::format("{}.zip", dir_to_add.filename().string());
		if (fs::exists(zip_name)) { util.rm_rf(zip_name); }
		auto const args = build_args(zip_name, dir_to_add.string());
		if (!execute(args)) { throw Panic{std::format("Failed to create ZIP {}", zip_name)}; }
		return zip_name;
	}

  private:
	[[nodiscard]] static auto build_args(std::string_view const zip_name, std::string_view const dir_to_add) -> std::string {
#if defined(_WIN32)
		return std::format("-acf {} {}", zip_name, dir_to_add);
#else
		return std::format("-r {} {}", zip_name, dir_to_add);
#endif
	}
};
} // namespace dz::detail
