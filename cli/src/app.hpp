#pragma once
#include "clap/result.hpp"
#include "depzip/instance.hpp"
#include "djson//json.hpp"
#include "klib/log/typed.hpp"

namespace depzip::cli {
class App {
  public:
	[[nodiscard]] auto run(int argc, char const* const* argv) -> int;

  private:
	[[nodiscard]] auto parse_args(int argc, char const* const* argv) -> clap::Result;
	void read_manifest();
	void run();

	klib::log::Typed<App> m_log{};

	std::string_view m_manifest_path{};
	InstanceCreateInfo m_instance_ci{};
	Config m_config{};

	dj::Json m_manifest_json{};
	Manifest m_manifest{};

	std::unique_ptr<Instance> m_instance{};
};
} // namespace depzip::cli
