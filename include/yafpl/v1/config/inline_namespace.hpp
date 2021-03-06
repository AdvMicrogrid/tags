#ifndef YAFPL_CONFIG_INLINE_NAMESPACE_HPP
#define YAFPL_CONFIG_INLINE_NAMESPACE_HPP

//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/config.hpp>

#if !defined(BOOST_NO_CXX11_INLINE_NAMESPACES)
# define YAFPL_INLINE_NAMESPACE(name) inline namespace name
# define YAFPL_DCL_INLINE_NAMESPACE(name) inline namespace name {}
#else
# define YAFPL_INLINE_NAMESPACE(name) namespace name
# define YAFPL_DCL_INLINE_NAMESPACE(name) namespace name {} using namespace name;
#endif


#endif
