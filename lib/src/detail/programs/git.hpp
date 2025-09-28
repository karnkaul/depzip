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

	void clone(Clone const& params) const {
		if (fs::exists(params.dest_dir)) { util.rm_rf(params.dest_dir); }
		auto builder = StringBuilder{.value = std::format("{} --depth={}", Clone::name_v, params.depth)};
		if (!params.tag.empty()) { builder.append(std::format("--branch={}", params.tag)); }
		builder.append(params.url).append(params.dest_dir.string());
		if (!execute(builder.value)) { throw Panic{std::format("Failed to clone {}@{}", params.tag, params.url)}; }
	}
};
} // namespace dz::detail
