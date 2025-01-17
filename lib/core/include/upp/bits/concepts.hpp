#pragma once

#include <type_traits>

namespace upp::concepts {

template <class T>
concept enum_type = std::is_enum_v<T>;

}
