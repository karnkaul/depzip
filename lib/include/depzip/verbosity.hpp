#pragma once
#include <cstdint>

namespace dz {
/// \brief Output verbosity.
enum class Verbosity : std::int8_t {
	/// \brief Host command outputs.
	Default,
	/// \brief No output.
	Silent,
	/// \brief Host command outputs and action logs.
	Verbose
};
} // namespace dz
