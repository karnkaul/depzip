#pragma once
#include <depzip/package_info.hpp>
#include <depzip/verbosity.hpp>
#include <memory>
#include <span>

namespace dz {
struct Config {
	std::string_view working_dir{"."};
	std::string_view source_dir{"src"};
	Verbosity verbosity{Verbosity::Default};
};

class Instance {
  public:
	Instance(Instance const&) = delete;
	Instance(Instance&&) = delete;
	auto operator=(Instance const&) = delete;
	auto operator=(Instance&&) = delete;

	Instance() = default;
	virtual ~Instance() = default;

	virtual void vendor(std::span<PackageInfo const> packages, Config const& config = {}) = 0;
};

[[nodiscard]] auto create_instance() -> std::unique_ptr<Instance>;
} // namespace dz
