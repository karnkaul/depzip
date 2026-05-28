#pragma once
#include "klib/log/tagged.hpp"

namespace depzip::detail {
auto const log = klib::log::Tagged{"depzip"};

namespace shell {
auto const log = klib::log::Tagged{"depzip::shell"};
} // namespace shell

namespace util {
auto const log = klib::log::Tagged{"depzip::util"};
} // namespace util
} // namespace depzip::detail
