#include "depzip/instance.hpp"
#include "depzip/json_io.hpp"
#include "depzip/panic.hpp"
#include "detail/log.hpp"
#include "detail/package.hpp"
#include "detail/programs/git.hpp"
#include "detail/programs/zip.hpp"
#include "detail/shell.hpp"
#include "detail/string_builder.hpp"
#include "detail/workspace.hpp"
#include <unordered_map>

namespace depzip::detail {
namespace {
[[nodiscard]] auto contains_host(fs::path uri) -> bool {
	uri = uri.parent_path();
	if (uri.empty()) { return true; }
	return uri.string().find_first_of(":./") != std::string::npos;
}

constexpr auto dev_null_v = std::string_view{
#if defined(_WIN32) && !defined(__MINGW__)
	" >nul 2>nul"
#else
	"> /dev/null 2>&1"
#endif
};

class Instance : public depzip::Instance {
	void vendor(Manifest const& manifest, Config const& config) final {
		setup(config);

		if (manifest.packages.empty()) {
			throw Panic{"Nothing to vendor"};
			return;
		}

		m_git.host.set_value(manifest.default_host);
		for (auto const& package_info : manifest.packages) { add_package(package_info); }
		create_zip();
	}

	void setup(Config const& config) { m_workspace.setup(config.working_dir, config.source_dir); }

	void add_package(PackageInfo const& package_info) {
		auto const& package = m_packages.emplace_back(m_git, m_workspace.get_src_dir(), package_info);
		log.info("== Package setup complete: {}", package.get_subdir().generic_string());
	}

	void create_zip() {
		auto const zip_name = m_zip.create_archive(m_workspace.get_src_dir());
		log.info("== ZIP file {} created", zip_name);
	}

	Workspace m_workspace{};

	Git m_git{};
	Zip m_zip{};

	std::vector<Package> m_packages{};
};
} // namespace

auto StringBuilder::append(std::string_view const text) -> StringBuilder& {
	if (text.empty()) { return *this; }
	if (value.empty()) {
		value = text;
	} else {
		std::format_to(std::back_inserter(value), " {}", text);
	}
	return *this;
}

auto shell::execute(std::string_view const command, std::string_view const args) -> Result {
	auto expr = StringBuilder::build(command, args);
	log.debug("{}", expr);
	expr.append(dev_null_v);
	return std::system(expr.c_str()); // NOLINT(concurrency-mt-unsafe)
}

auto shell::execute_silent(std::string_view const command, std::string_view const args) -> Result {
	auto const expr = StringBuilder::build(command, args, dev_null_v);
	return std::system(expr.c_str()); // NOLINT(concurrency-mt-unsafe)
}

void util::mkdir(fs::path const& path) {
	if (path == ".") { return; }
	log.debug("creating directory {}", path.generic_string());
	if (!fs::create_directories(path)) { throw Panic{std::format("Failed to create directory {}", path.generic_string())}; }
}

void util::cd(fs::path const& path) {
	if (path.empty() || path == ".") { return; }
	log.debug("changing pwd to {}", path.generic_string());
	fs::current_path(path);
}

void util::rm_rf(fs::path const& path) {
	if (!fs::exists(path)) { return; }
	log.debug("deleting {}", path.generic_string());

	auto const extract_failed_path = [](std::string_view const fs_err) -> std::string {
		auto const lbrace = fs_err.find_last_of('[');
		if (lbrace == std::string_view::npos) { return {}; }
		auto ret = fs_err.substr(lbrace + 1);
		auto const rbrace = ret.find(']');
		if (rbrace < ret.size()) { ret.remove_suffix(ret.size() - rbrace); }
		return std::string{ret};
	};

	static constexpr auto max_iterations_v{100};
	auto retry_map = std::unordered_map<std::string, int>{};
	auto iteration = 1;
	for (; iteration <= max_iterations_v; ++iteration) {
		try {
			fs::remove_all(path);
			return;
		} catch (fs::filesystem_error const& e) {
			if (iteration >= max_iterations_v) {
				// loop is over, just throw.
				break;
			}

			auto const failed_path = extract_failed_path(e.what());
			if (failed_path.empty()) {
				// can't fix perms if there's no path.
				std::rethrow_exception(std::current_exception());
			}

			auto const retry_attempt = ++retry_map[failed_path];
			static constexpr auto max_attempts_v{10};
			if (retry_attempt >= max_attempts_v) {
				// problematic path has exhausted retry attempts.
				std::rethrow_exception(std::current_exception());
			}

			log.debug("{}\n  changing perms and retrying... (attempt: {}/{}, iteration: {}/{})", e.what(), retry_attempt, max_attempts_v, iteration,
					  max_iterations_v);
			fs::permissions(failed_path, fs::perms::owner_write | fs::perms::others_write);
		}
	}

	throw Panic{std::format("Failed to delete {} ({} iterations)", path.generic_string(), iteration)};
}

Program::Program(std::string_view const command, std::string_view const does_exist_args) : m_command(command) {
	if (!shell::execute_silent(command, does_exist_args)) { throw Panic{std::format("{} not found", get_command())}; }
}

auto Program::execute(std::string_view const args) const -> bool { return shell::execute(m_command, args).is_success(); }

void Git::Host::set_value(std::string_view const value) {
	if (value.empty()) { return; }
	m_value = value;
	if (m_value.ends_with('/')) { m_value.pop_back(); }
}

auto Git::Host::to_url(std::string_view const uri) const -> std::string {
	if (contains_host(uri)) { return std::string{uri}; }
	return std::format("{}/{}", m_value, uri);
}

void Git::clone(Clone const& params) const {
	if (fs::exists(params.dest_dir)) { util::rm_rf(params.dest_dir); }
	auto builder = StringBuilder{.value = std::format("{} --depth={}", Clone::name_v, params.depth)};
	if (!params.branch.empty()) { builder.append(std::format("--branch={}", params.branch)); }
	auto const url = host.to_url(params.uri);
	builder.append(url).append(params.dest_dir.string());
	if (!execute(builder.value)) {
		if (params.branch.empty()) { throw Panic{std::format("Failed to clone {}", url)}; }
		throw Panic{std::format("Failed to clone {} (branch: {})", url, params.branch)};
	}
}

auto Zip::create_archive(fs::path const& dir_to_add) const -> std::string {
	auto const zip_name = std::format("{}.zip", dir_to_add.filename().string());
	if (fs::exists(zip_name)) { util::rm_rf(zip_name); }
	auto const args = build_args(zip_name, dir_to_add.string());
	if (!execute(args)) { throw Panic{std::format("Failed to create ZIP {}", zip_name)}; }
	return zip_name;
}

auto Zip::build_args(std::string_view const zip_name, std::string_view const dir_to_add) -> std::string {
#if defined(_WIN32)
	return std::format("-acf {} {}", zip_name, dir_to_add);
#else
	return std::format("-r {} {}", zip_name, dir_to_add);
#endif
}

Package::Package(Git const& git, fs::path const& src_dir, Info const& info) {
	m_subdir = fs::path{info.subdir};
	if (m_subdir.empty()) { m_subdir = fs::path{info.uri}.stem(); }

	auto const clone_params = Git::Clone{
		.uri = info.uri,
		.branch = info.branch,
		.dest_dir = src_dir / m_subdir,
	};
	git.clone(clone_params);

	util::rm_rf(clone_params.dest_dir / ".git");
	for (auto const& subpath : info.remove_subpaths) {
		auto const path = clone_params.dest_dir / subpath;
		util::rm_rf(path);
	}

	if (info.custom_command.empty()) { return; }
	auto const result = shell::execute(info.custom_command);
	if (!result) { throw Panic{std::format("Failed to execute custom command for {} (exit code: {})", get_subdir().generic_string(), result.get_code())}; }

	log.info("== Package setup complete: {}", get_subdir().generic_string());
}
} // namespace depzip::detail

auto depzip::create_instance() -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(); }

void depzip::from_json(dj::Json const& json, PackageInfo& package) {
	from_json(json["uri"], package.uri);
	from_json(json["branch"], package.branch, package.branch);
	from_json(json["subdir"], package.subdir, package.subdir);
	for (auto const& subpath : json["remove_subpaths"].as_array()) { from_json(subpath, package.remove_subpaths.emplace_back()); }
	from_json(json["custom_command"], package.custom_command);
}

void depzip::to_json(dj::Json& json, PackageInfo const& package) {
	if (!package.uri.empty()) { to_json(json["uri"], package.uri); }
	if (!package.branch.empty()) { to_json(json["branch"], package.branch); }
	if (!package.subdir.empty()) { to_json(json["subdir"], package.subdir); }
	for (auto const subpath : package.remove_subpaths) { to_json(json["remove_subpaths"].push_back(), subpath); }
	if (!package.custom_command.empty()) { to_json(json["custom_command"], package.custom_command); }
}

void depzip::from_json(dj::Json const& json, Manifest& manifest) {
	for (auto const& package : json["packages"].as_array()) { from_json(package, manifest.packages.emplace_back()); }
	from_json(json["default_host"], manifest.default_host);
}

void depzip::to_json(dj::Json& json, Manifest const& manifest) {
	for (auto const& package : manifest.packages) { to_json(json["packages"].push_back(), package); }
	if (!manifest.default_host.empty()) { to_json(json["default_host"], manifest.default_host); }
}
