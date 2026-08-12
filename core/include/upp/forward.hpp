#pragma once

/**
 * \brief Shorthand for replacing std::forward
 * */
#define UPP_FWD(obj) std::forward<decltype(obj)>(obj)
