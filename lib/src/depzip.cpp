#include <depzip/instance.hpp>
#include <depzip/json_io.hpp>
#include <depzip/panic.hpp>
#include <detail/logger.hpp>
#include <detail/package.hpp>
#include <detail/programs/git.hpp>
#include <detail/programs/zip.hpp>
#include <detail/string_builder.hpp>
#include <detail/workspace.hpp>
#include <unordered_map>

namespace dz::detail {
namespace {
class Instance : public dz::Instance {
	void vendor(std::span<PackageInfo const> packages, Config const& config) final {
		setup(config);

		if (packages.empty()) {
			std::println("Nothing to vendor");
			return;
		}

		for (auto const& package_info : packages) { add_package(package_info); }
		create_zip();
	}

	void setup(Config const& config) {
		m_logger.verbosity = config.verbosity;
		m_workspace.setup(config.working_dir, config.source_dir);
	}

	void add_package(PackageInfo const& package_info) {
		auto const& package = m_packages.emplace_back(m_git, m_workspace.get_src_dir(), package_info);
		m_logger("== Package setup complete: {}\n", package.get_subdir().generic_string());
	}

	void create_zip() {
		auto const zip_name = m_zip.create_archive(m_workspace.get_src_dir());
		m_logger("== ZIP file {} created\n", zip_name);
	}

	Logger m_logger{};
	Util m_util{m_logger};
	Workspace m_workspace{m_util};

	Git m_git{m_util};
	Zip m_zip{m_util};

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

auto Shell::execute(std::string_view const args, Verbosity const verbosity) const -> Result {
	auto const line = StringBuilder{.value = m_command}.append(args, create_suffix(verbosity)).value;
	print_if_verbose(line, verbosity);
	return std::system(line.c_str()); // NOLINT(concurrency-mt-unsafe)
}

auto Shell::create_suffix(Verbosity const verbosity) -> std::string_view {
	if (verbosity != Verbosity::Silent) { return {}; }
#if defined(_WIN32) && !defined(__MINGW__)
	return " >nul 2>nul";
#else
	return "> /dev/null 2>&1";
#endif
}

void Shell::print_if_verbose(std::string_view const line, Verbosity const verbosity) {
	if (verbosity != Verbosity::Verbose) { return; }
	std::println("-- {}", line);
}

void Util::mkdir(fs::path const& path) const {
	if (path == ".") { return; }
	logger("-- Creating directory {}", path.generic_string());
	if (!fs::create_directories(path)) { throw Panic{std::format("Failed to create directory {}", path.generic_string())}; }
}

void Util::cd(fs::path const& path) const {
	if (path.empty() || path == ".") { return; }
	logger("-- Changing pwd to {}", path.generic_string());
	fs::current_path(path);
}

void Util::rm_rf(fs::path const& path) const {
	if (!fs::exists(path)) { return; }
	logger("-- Deleting {}", path.generic_string());

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

			logger("{}\n  changing perms and retrying... (attempt: {}/{}, iteration: {}/{})", e.what(), retry_attempt, max_attempts_v, iteration,
				   max_iterations_v);
			fs::permissions(failed_path, fs::perms::owner_write | fs::perms::others_write);
		}
	}

	throw Panic{std::format("Failed to delete {} ({} iterations)", path.generic_string(), iteration)};
}

Program::Program(Util const& util, std::string command, std::string_view const does_exist_args) : Shell(std::move(command)), util(util) {
	if (!Shell::execute(does_exist_args, Verbosity::Silent)) { throw Panic{std::format("{} not found", get_command())}; }
}

auto Program::execute(std::string_view const args) const -> Result { return Shell::execute(args, util.logger.verbosity); }

void Git::clone(Clone const& params) const {
	if (fs::exists(params.dest_dir)) { util.rm_rf(params.dest_dir); }
	auto builder = StringBuilder{.value = std::format("{} --depth={}", Clone::name_v, params.depth)};
	if (!params.tag.empty()) { builder.append(std::format("--branch={}", params.tag)); }
	builder.append(params.url).append(params.dest_dir.string());
	if (!execute(builder.value)) { throw Panic{std::format("Failed to clone {}@{}", params.tag, params.url)}; }
}

auto Zip::create_archive(fs::path const& dir_to_add) const -> std::string {
	auto const zip_name = std::format("{}.zip", dir_to_add.filename().string());
	if (fs::exists(zip_name)) { util.rm_rf(zip_name); }
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
	m_subdir = fs::path{info.subdir_name};
	if (m_subdir.empty()) { m_subdir = fs::path{info.repo_uri}.filename(); }

	auto const clone_params = Git::Clone{
		.url = std::format("{}{}", info.repo_provider, info.repo_uri),
		.tag = info.git_tag,
		.dest_dir = src_dir / m_subdir,
	};
	git.clone(clone_params);

	git.util.rm_rf(clone_params.dest_dir / ".git");
	for (auto const& subpath : info.remove_subpaths) {
		auto const path = clone_params.dest_dir / subpath;
		git.util.rm_rf(path);
	}
}
} // namespace dz::detail

auto dz::create_instance() -> std::unique_ptr<Instance> { return std::make_unique<detail::Instance>(); }

void dz::from_json(dj::Json const& json, PackageInfo& package) {
	if (json.is_string()) {
		from_json(json, package.repo_uri);
	} else {
		from_json(json["repo_uri"], package.repo_uri);
		from_json(json["repo_provider"], package.repo_provider, package.repo_provider);
		from_json(json["git_tag"], package.git_tag, package.git_tag);
		from_json(json["subdir_name"], package.subdir_name, package.subdir_name);
		for (auto const& subpath : json["remove_subpaths"].as_array()) { from_json(subpath, package.remove_subpaths.emplace_back()); }
	}
}

void dz::to_json(dj::Json& json, PackageInfo const& package) {
	to_json(json["repo_uri"], package.repo_uri);
	to_json(json["repo_provider"], package.repo_provider);
	to_json(json["git_tag"], package.git_tag);
	to_json(json["subdir_name"], package.subdir_name);
	for (auto const subpath : package.remove_subpaths) { to_json(json["remove_subpaths"].push_back(), subpath); }
}

void dz::from_json(dj::Json const& json, Manifest& manifest) {
	for (auto const& package : json["packages"].as_array()) { from_json(package, manifest.packages.emplace_back()); }
}

void dz::to_json(dj::Json& json, Manifest const& manifest) {
	for (auto const& package : manifest.packages) { to_json(json["packages"].push_back(), package); }
}
