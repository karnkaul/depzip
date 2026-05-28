#pragma once
#include "depzip/build_version.hpp"
#include "depzip/config.hpp"
#include "depzip/manifest.hpp"
#include "klib/base_types.hpp"
#include "klib/task/queue_create_info.hpp"
#include <memory>

namespace depzip {
struct InstanceCreateInfo {
	/// \brief Number of threads to use.
	klib::task::ThreadCount thread_count{klib::task::get_max_threads()};
};

/// \brief Opaque interface for primary API.
class Instance : public klib::Polymorphic, public klib::Pinned {
  public:
	/// \brief Clone packages and create ZIP archive.
	/// Throws Panic on fatal errors.
	/// \param manifest Manifest description.
	/// \param config Vendoring configuration.
	virtual void vendor(Manifest const& manifest, Config const& config = {}) noexcept(false) = 0;
};

/// \returns A concrete Instance.
[[nodiscard]] auto create_instance(InstanceCreateInfo const& create_info = {}) -> std::unique_ptr<Instance>;
} // namespace depzip
