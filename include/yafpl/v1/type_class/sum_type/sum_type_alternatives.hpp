//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef YAFPL_V1_SUM_TYPE_ALTERNATIVES_HPP
#define YAFPL_V1_SUM_TYPE_ALTERNATIVES_HPP

#include <yafpl/v1/config.hpp>

namespace yafpl
{
YAFPL_INLINE_NAMESPACE(v1)
{
  // By a default a single type is a sum type of itself
  template <class ST>
  struct sum_type_alternatives: meta::identity<meta::types<ST>>
  {
  };

  template <class ST>
  using sum_type_alternatives_t = typename sum_type_alternatives<ST>::type;
}
}
#endif // header
