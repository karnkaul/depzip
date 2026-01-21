#pragma once
#include "depzip/instance.hpp"
#include <djson/json.hpp>
#include <klib/args/parse_result.hpp>
#include <klib/log.hpp>

namespace dz::cli {
class App {
  public:
	[[nodiscard]] auto run(int argc, char const* const* argv) -> int;

  private:
	[[nodiscard]] auto parse_args(int argc, char const* const* argv) -> klib::args::ParseResult;
	void read_manifest();
	void run();

	klib::TypedLogger<App> m_log{};

	std::string_view m_manifest_path{};
	Config m_config{};

	dj::Json m_manifest_json{};
	Manifest m_manifest{};

	std::unique_ptr<Instance> m_instance{};
};
} // namespace dz::cli
