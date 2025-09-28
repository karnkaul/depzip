#pragma once
#include <stdexcept>

namespace dz {
/// \brief Library exception type.
class Panic : public std::runtime_error {
  public:
	using std::runtime_error::runtime_error;
};
} // namespace dz
