//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompboost::anying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#include <data_type/v0/maybe.hpp>

#include <string>
#include <functional>

#include <boost/detail/lightweight_test.hpp>

struct X{};

int main()
{
  using namespace meta;
  using namespace functional;
  using namespace data_type;
  {
    maybe<int> a = just(2);
    functional::match<void>(a,
        [](just_t<int> i)
        {},
        [](nothing_t i)
        {
          BOOST_TEST(false);
        }
    );
  }
  {
    maybe<int> a = just(2);
    functional::match<void>(a,
        [](just_t<int> i)
        {},
        [](...)
        {
          BOOST_TEST(false);
        }
    );
  }
  {
    maybe<int> a = nothing;
    functional::match<void>(a,
        [](just_t<int> i)
        {
          BOOST_TEST(false);
        },
        [](nothing_t i)
        {
        }
    );
  }
  {
    maybe<int> a;
    functional::match<void>(a,
        [](just_t<int> i)
        {
          BOOST_TEST(false);
        },
        [](nothing_t i)
        {
        }
    );
  }

  {
    int i = 0;
    maybe<int*> a = just(&i);
    functional::match<void>(a,
        [](just_t<int*> i)
        {},
        [](nothing_t const&i)
        {
          BOOST_TEST(false);
        }
    );
  }
  {
    int i = 0;
    maybe<std::reference_wrapper<int>> a = just(std::ref(i));
    functional::match<void>(a,
        [](just_t<std::reference_wrapper<int>> i)
        {},
        [](nothing_t const&i)
        {
          BOOST_TEST(false);
        }
    );
  }
  {
    maybe<int> a = just(2);
    maybe<int> b = just(2);
    functional::match_all<void>(std::make_tuple(a, b),
        [](just_t<int> i, just_t<int> j )
        {
        },
        [](auto const &i, auto const &j )
        {
          BOOST_TEST(false);
        }
    );
  }

  {
    maybe<int> a = just(2);
    maybe<int> b = nothing;
    functional::match_all<void>(std::make_tuple(a, b),
        [](just_t<int> i, just_t<int> j )
        {
          BOOST_TEST(false);
        },
        [](auto const &i, auto const &j )
        {
        },
        [](...)
        {
          BOOST_TEST(false);
        }
    );
  }
  {
    maybe<int> a = just(2);
    maybe<int> b = nothing;
    functional::match_all<void>(std::make_tuple(a, b),
        [](just_t<int> i, just_t<int> j )
        {
          BOOST_TEST(false);
        },
        [](...)
        {
        }
    );
  }


#if 0
  // compile fail as there is no overload for 'nothing_t'
  {
    maybe<int> a;
    boost::match<void>(a,
        [](just_t<int> i)
        {
          BOOST_TEST(false);
        }
    );
  }
#endif

  return boost::report_errors();
}

