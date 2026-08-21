#include "app.hpp"
#include "clap/parser.hpp"
#include "depzip/build_version.hpp"
#include "depzip/json_io.hpp"
#include "depzip/panic.hpp"
#include "klib/debug/assert.hpp"
#include "klib/file_io.hpp"
#include <filesystem>

namespace depzip::cli {
namespace {
namespace fs = std::filesystem;

[[nodiscard]] auto resolved_path(fs::path const& in) -> fs::path {
	KLIB_ASSERT(!in.empty());
	if (!fs::is_symlink(in)) { return in; }

	auto ret = klib::resolve_symlink(in.string());
	if (ret.empty()) { throw Panic{std::format("Unresolved symlink: {}", in.generic_string())}; }
	return ret;
}

[[nodiscard]] auto locate_manifest(fs::path in) -> fs::path {
	if (!in.empty()) {
		in = resolved_path(in);
		if (!fs::exists(in)) { throw Panic{std::format("Nonexistent depzip manifest: {}", in.string())}; }
		if (!fs::is_regular_file(in)) { throw Panic{std::format("depzip manifest is not a file: {}", in.string())}; }
		return in;
	}

	using namespace std::string_view_literals;
	static constexpr auto default_paths_v = std::array{
		"depzip.json"sv,
		"depzip.jsonc"sv,
	};
	for (auto const path : default_paths_v) {
		if (fs::is_regular_file(path)) { return path; }
	}

	throw Panic{"Failed to locate depzip manifest"};
}
} // namespace

auto App::run(int const argc, char const* const* argv) -> int {
	auto const parse_result = parse_args(argc, argv);
	if (parse_result.should_early_exit()) { return parse_result.return_code(); }

	run();
	return EXIT_SUCCESS;
}

auto App::parse_args(int const argc, char const* const* argv) -> clap::Result {
	static auto const version_str = std::format("{}", build_version_v);
	auto parse_info = clap::spec::Parameters{
		.program = clap::Program{.version = version_str},
	};
	auto thread_count = std::to_underlying(m_instance_ci.thread_count);
	parse_info.parameters = {
		clap::named_option(m_config.source_dir, "s,src", "source directory"),
		clap::named_option(m_config.working_dir, "w,pwd", "working directory"),
		clap::named_option(thread_count, "j,jobs", "job/thread count"),
		clap::positional_optional(m_manifest_path, "manifest", "path to manifest"),
	};
	auto parser = clap::Parser{std::move(parse_info)};
	auto const ret = parser.parse_main(argc, argv);
	m_instance_ci.thread_count = klib::task::ThreadCount{thread_count};
	return ret;
}

void App::read_manifest() {
	auto const manifest_path = locate_manifest(m_manifest_path);
	KLIB_ASSERT(!manifest_path.empty());

	auto result = dj::Json::from_file(manifest_path.string(), dj::ParseMode::Jsonc);
	if (!result) {
		auto const error = dj::to_string(result.error());
		throw Panic{std::format("Failed to read manifest: {}\n{}", manifest_path.string(), error)};
	}
	m_manifest_json = std::move(*result);
	from_json(m_manifest_json, m_manifest);
}

void App::run() {
	m_log.info("depzip {}", build_version_v);

	read_manifest();
	if (m_manifest.packages.empty()) {
		m_log.info("Nothing to vendor");
		return;
	}

	m_instance = create_instance(m_instance_ci);
	m_instance->vendor(m_manifest, m_config);
}
} // namespace depzip::cli
