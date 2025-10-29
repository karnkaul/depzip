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

	explicit Zip(Util const& util) : Program(util, name_v) {}

	[[nodiscard]] auto create_archive(fs::path const& dir_to_add) const -> std::string;

  private:
	[[nodiscard]] static auto build_args(std::string_view zip_name, std::string_view dir_to_add) -> std::string;
};
} // namespace dz::detail
