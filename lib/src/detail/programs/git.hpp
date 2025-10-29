#pragma once
#include <detail/program.hpp>
#include <detail/util.hpp>

namespace dz::detail {
class Git : public Program {
  public:
	static constexpr std::string_view name_v{"git"};

	class Host {
	  public:
		static constexpr std::string_view default_host_v{"https://github.com"};

		void set_value(std::string_view value);
		[[nodiscard]] auto get_value() const -> std::string_view { return m_value; }

		[[nodiscard]] auto to_url(std::string_view uri) const -> std::string;

	  private:
		std::string m_value{default_host_v};
	};

	struct Clone {
		static constexpr std::string_view name_v{"clone"};

		std::string_view uri{};
		std::string_view branch{};
		fs::path dest_dir{};

		int depth{1};
	};

	explicit Git(Util const& util) : Program(util, name_v) {}

	void clone(Clone const& params) const;

	Host host{};
};
} // namespace dz::detail
