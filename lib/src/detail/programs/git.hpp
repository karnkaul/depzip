#pragma once
#include <detail/program.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Git : public Program {
  public:
	static constexpr std::string_view name_v{"git"};

	struct Clone {
		static constexpr std::string_view name_v{"clone"};

		std::string url{};
		std::string_view tag{};
		fs::path dest_dir{};

		int depth{1};
	};

	explicit Git(Util const& util) : Program(util, std::string{name_v}) {}

	void clone(Clone const& params) const;
};
} // namespace dz::detail
