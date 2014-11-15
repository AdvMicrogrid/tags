//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef META_V0_IDENTITY_HPP
#define META_V0_IDENTITY_HPP

#include <meta/v0/config.hpp>

namespace meta {
  CONFIG_INLINE_NAMESPACE(v0) {

    template <typename T>
    struct identity { using type = T; };

  }
}

#endif // header
