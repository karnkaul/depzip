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

	explicit Git() : Program(std::string{name_v}) {}

	void clone(Clone const& params) const {
		if (fs::exists(params.dest_dir)) { util::rm_rf(verbosity, params.dest_dir); }
		auto const args =
			StringBuilder{.value = std::format("{} {} --depth={}", Clone::name_v, params.url, params.depth)}.append(params.dest_dir.string()).value;
		if (!execute(args)) { throw Panic{std::format("Failed to clone {}@{}", params.tag, params.url)}; }
	}
};
} // namespace dz::detail
