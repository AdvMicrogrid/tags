//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#include <tags/v0/tag_any.hpp>

#include <string>

#include <boost/detail/lightweight_test.hpp>

struct t1
{};

struct t2
{};

namespace tags
{
  template<>
  struct tag_type<t1>
  {
    typedef int type;
  };
  template<>
  struct tag_type<t2>
  {
    typedef std::string type;
  };
}

int main()
{
  using namespace boost;
  using namespace tags;

  {
    // example
    tag_any ta = make_tag_any<t1>(1);
    tag_accept<void, t1, t2>(ta,
        [](t1, int& i)
        {},
        [](t2, std::string& i)
        {}
    );
  }
  {
    // example
    tag_any ta = make_tag_any<t1>(1);
    tag_accept<void>(ta, types<t1, t2>(),
        [](t1, int& i)
        {},
        [](t2, std::string& i)
        {}
    );
  }
  {
    // example
    any a = make_tag_any<t1>(1);
    tag_accept<void, t1>(a,
        [](t1 , int& i)
        {}
    );
  }
  {
    // example
    any a = make_tag_any<t1>(1);
    tag_accept<void>(a, types<t1>(),
        [](t1 , int& i)
        {}
    );
  }
  return boost::report_errors();
}

