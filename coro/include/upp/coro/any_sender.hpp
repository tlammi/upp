#pragma once

#include <exec/any_sender_of.hpp>
#include <stdexec/execution.hpp>

namespace upp::coro {

namespace detail {

using set_value = stdexec::set_value_t;
using set_error = stdexec::set_error_t;
using set_stopped = stdexec::set_stopped_t;

using stdexec::completion_signatures;

using query = exec::queries<>;
using sender_query = exec::queries<>;

template <class Completion>
using any_sender_impl =
    exec::any_sender<exec::any_receiver<Completion, query>, sender_query>;

template <class... Ts>
using completion =
    completion_signatures<set_value(Ts...), set_error(std::exception_ptr),
                          set_stopped()>;

template <class... Ts>
using any_sender = any_sender_impl<completion<Ts...>>;

template <class... Ts>
using trivial_completion = completion_signatures<set_value(Ts...)>;

template <class... Ts>
using trivial_any_sender = any_sender_impl<trivial_completion<Ts...>>;

template <class... Ts>
using unstoppable_completion =
    completion_signatures<set_value(Ts...), set_error(std::exception_ptr)>;

template <class... Ts>
using unstoppable_any_sender = any_sender_impl<unstoppable_completion<Ts...>>;

template<class... Ts>
using infallible_completion = completion_signatures<set_value(Ts...), set_stopped()>;

template<class... Ts>
using infallible_any_sender = any_sender_impl<infallible_completion<Ts...>>;

}  // namespace detail

// Can finish with value, error or cancellation
using detail::any_sender;

// Can finish with only a value
using detail::trivial_any_sender;

// Can finish with value or error (no cancellation)
using detail::unstoppable_any_sender;

// Can finish with value or cancellation (no error)
using detail::infallible_any_sender;

}  // namespace upp::coro
