//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef FUCTIONAL_V0_APPLY_HPP
#define FUCTIONAL_V0_APPLY_HPP

#include <functional/v0/config.hpp>
#include <utility>
#include <type_traits>
#include <tuple>

namespace functional {
  CONFIG_INLINE_NAMESPACE(v0) {

    namespace detail {

      template <class Function, class Tuple, std::size_t... i>
      decltype(auto) apply_impl(Function &&fn,
                                Tuple &&tuple,
                                std::index_sequence<i...>) {
        return std::forward<Function>(fn)(std::get<i>(std::forward<Tuple>(tuple))...);
      }

    } // detail

    /* C++17 std::apply */

    template <class Function, class Tuple>
    decltype(auto) apply(Function &&fn, Tuple &&tuple) {
      return detail::apply_impl(std::forward<Function>(fn),
                        std::forward<Tuple>(tuple),
                        std::make_index_sequence<
                        std::tuple_size<std::decay_t<Tuple>>::value>());
    }

  }
}

#endif // header
