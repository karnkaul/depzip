#pragma once
#include <depzip/package_info.hpp>
#include <depzip/verbosity.hpp>
#include <memory>
#include <span>

namespace dz {
/// \brief Configuration for a vendor request.
struct Config {
	/// \brief Working directory.
	std::string_view working_dir{"."};
	/// \brief Source directory.
	/// All repositories are cloned here.
	/// This is the directory that gets archived into a ZIP file.
	std::string_view source_dir{"src"};
	/// \brief Output verbosity.
	Verbosity verbosity{Verbosity::Default};
};

/// \brief Opaque interface for primary API.
class Instance {
  public:
	Instance(Instance const&) = delete;
	Instance(Instance&&) = delete;
	auto operator=(Instance const&) = delete;
	auto operator=(Instance&&) = delete;

	Instance() = default;
	virtual ~Instance() = default;

	/// \brief Clone packages and create ZIP archive.
	/// Throws Panic on fatal errors.
	/// \param pacakges Packages to clone and include in the archive.
	/// \param config Vendoring configuration.
	virtual void vendor(std::span<PackageInfo const> packages, Config const& config = {}) noexcept(false) = 0;
};

/// \returns A concrete Instance.
[[nodiscard]] auto create_instance() -> std::unique_ptr<Instance>;
} // namespace dz
