//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Vicente J. Botet Escriba 2014-2015.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file // LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
//////////////////////////////////////////////////////////////////////////////

#ifndef YAFPL_FUCTIONAL_V0_MATCH_HPP
#define YAFPL_FUCTIONAL_V0_MATCH_HPP

#include <yafpl/v1/config.hpp>

#include <yafpl/v1/functional/apply.hpp>
#include <yafpl/v1/functional/overload.hpp>
#include <yafpl/v1/meta/identity.hpp>
#include <yafpl/v1/meta/id.hpp>
#include <yafpl/v1/meta/types.hpp>
#include <yafpl/v1/meta/void_.hpp>
#include <yafpl/v1/type_class/sum_type/sum_type_alternatives.hpp>

#include <utility>
#include <type_traits>
#include <tuple>

namespace yafpl
{

  YAFPL_INLINE_NAMESPACE(v1)
  {

    using meta::types;
    using meta::id;

#if ! defined YAFPL_X1
      template <class R, class T, class F>
      R match(yafpl::meta::id<R>, T const& x, F&& f)
      {
        return f(x);
      }
#else
    template <class T, class F>
    auto match(T const& x, F&& f) -> decltype(f(x))
    {
      return f(x);
    }
#endif

    namespace detail
    {
      /**
       * Result: The result type of the overload
       * Functor: The nullary void function to call when there are no more alternatives in Types
       * DataTypeTuple: the product type of the current alternative of the sum types already visited
       * Sts: the product type of the sum types not yet visited
       * Types: the alternatives for the ST been visited
       *
       * Defines the operator(T const&) for each T in Types. This is the function called by match custom for ST
       */
      template <class Result, class Functor, class DataTypeTuple, class ST, class STs, class Ns>
      class applier;

      /**
       * Stores:
       * * where to store the result,
       * * the nullary void function to call when there are no more alternatives in Types,
       * * the product type of the reference to the current alternative of the sum types already visited
       */
      template <class R, class F, class ... DTs, class ST, class ... STs>
      class applier<R, F, std::tuple<DTs const&...>, ST, std::tuple<STs const&...>, std::index_sequence<> >
      {
      public:


        applier(R *r, F &fct, std::tuple<DTs const&...> &&members,
            std::tuple<STs const&...> &&sums)
        : fct(fct),
          r(r),
          members(std::move(members)),
          sums(std::move(sums))
        {}

        F &fct;
        R *r;
        std::tuple<DTs const&...> members;
        std::tuple<STs const&...> sums;

        // only here to make it possible in the derived class
        // using super::operator();
        void operator()() const
        {
        }
      };

      /**
       * Defines the operator(T const&). This is the function called by match custom for ST.
       * Inherits from the applier that will define the other operator(U) for U in Ts.
       */
      template <class R, class F, class... DTs, class ST, class... STs, std::size_t N, size_t... Ns>
      class applier<R, F, std::tuple<DTs const&...>, ST, std::tuple<STs const&...>, std::index_sequence<N, Ns...>>
      : public applier<R, F, std::tuple<DTs const&...>, ST, std::tuple<STs const&...>, std::index_sequence<Ns...>>
      {
      public:
        using T = sum_type_element_t<N, ST>;
        using super = applier<R, F, std::tuple<DTs const&...>, ST, std::tuple<STs const&...>, std::index_sequence<Ns...>>;

        /* Pass everything up to the base case. */
        applier(R *r, F &fct, std::tuple<DTs const&...> &&members, std::tuple<STs const&...> &&sums)
          : super(r, fct, std::move(members), std::move(sums))
        {}

        // make visible the other operators(U)
        using super::operator();

        void operator()(T const&v) const
        {
          dispatch(std::index_sequence_for<DTs...>(), v, std::index_sequence_for<STs...>());
        }

      private:
        /**
         * odts: the product of all the current references
         */
        template <class... ODTs>
        void dispatch_helper(
            std::tuple<ODTs const&...> &&odts) const
        {
          overload<R>(
              [&](auto *r)
              {
                *r = apply(this->fct, std::move(odts));
              },
              [&](void *)
              {
                apply(this->fct, std::move(odts));
              }
          )(this->r);
        }

        /**
         * odts: the product of all the current references up to sum type ST
         * sum, ostd... the sum types not yet visited
         */
        template <class... ODTs, class OST, class... OSTs>
        void dispatch_helper(std::tuple<ODTs const&...> &&odts, OST const& sum, OSTs const&... osts) const
        {
          // nested applier
          using applier_type = applier<R, F, std::tuple<ODTs const&...>, OST, std::tuple<OSTs const&...>,
              std::make_integer_sequence<std::size_t,sum_type_size<OST>::value >>;

          // customization point
#if ! defined YAFPL_X1
          match(id<R>{}, sum, applier_type(this->r, this->fct, std::move(odts), std::forward_as_tuple(osts...)));
#else
          match(sum, applier_type(this->r, this->fct, std::move(odts), std::forward_as_tuple(osts...)));
#endif
        }

        /**
         * This function is just a helper to manage with the indexes
         */
        template <size_t... i, size_t... j>
        void dispatch(std::index_sequence<i...>, T const& v, std::index_sequence<j...>) const
        {
          dispatch_helper(std::forward_as_tuple(std::get<i>(this->members)..., v), std::get<j>(this->sums)...);
        }
      };

      template <class R>
      struct storage
      {
        R get()
        { return std::move(r);}
        R *ptr()
        { return &r;}
        R r;
      };

      template <>
      struct storage<void>
      {
        void get() &&
        {}
        void *ptr()
        { return nullptr; }
      };

      /**
       * R: the result type
       * F the function object
       * ST: a sum type
       * STs: other sum types
       *
       * @effect calls to the match customization point for @c ST with a function object wrapping @c f and the other sum types @c osts.
       * This function object will end by forwarding the call to @c f with the stored alternative for each one of the @c st/@c sts.
       */
      template <class R, class F, class ST, class... STs>
      decltype(auto) apply_impl(F &&fct, ST const& sum, STs const&... osts)
      {
        using applier_type = applier<R, F, std::tuple<>, ST, std::tuple<STs const&...>,
            std::make_integer_sequence<std::size_t, sum_type_size<ST>::value> >;

        storage<R> r;
        // customization point
#if ! defined YAFPL_X1
        match(id<R>{}, sum, applier_type(r.ptr(), fct, std::forward_as_tuple(), std::forward_as_tuple(osts...)));
#else
        match(sum, applier_type(r.ptr(), fct, std::forward_as_tuple(), std::forward_as_tuple(osts...)));
#endif
        return std::move(r).get();
      }

      template <class T>
      struct is_tuple : std::false_type {};
      template <class T>
      struct is_tuple<T const> : is_tuple<T> {};
      template <class T>
      struct is_tuple<T volatile> : is_tuple<T> {};
      template <class T>
      struct is_tuple<T const volatile> : is_tuple<T> {};
      template <class ...Ts>
      struct is_tuple<std::tuple<Ts...>> : std::true_type {};

      template< class, class = void >
      struct has_result_type_member : std::false_type { };
      template< class T >
      struct has_result_type_member<T, meta::void_<typename T::result_type>> : std::true_type { };

      template< class T1, class T2 >
      struct and_;
      template< class T >
      struct and_<std::true_type, T> : T {};
      template< class T >
      struct and_<std::false_type, T> : std::false_type {};

      template< class T >
      struct have_result_type_member_x;
      template< template <class...> class TMPL, class T >
      struct have_result_type_member_x<TMPL<T>> : has_result_type_member<T> {};
      template< template <class...> class TMPL, class T1, class T2, class ...Ts >
      struct have_result_type_member_x<TMPL<T1, T2, Ts...>>
        : and_<
            has_result_type_member<T1>,
            have_result_type_member_x<TMPL<T2, Ts...>>
          >
      {};

      template<class ...Ts >
      struct have_result_type_member : have_result_type_member_x<types<Ts...>> { };

      template <class F, class ...Fs>
      struct deduced_result_type {
        using type =  typename F::result_type;
      };
      template <class ...Fs>
      using deduced_result_type_t = typename deduced_result_type<Fs...>::type;
    } // detail

    // explicit result type
    template <class R, class ST, class F1, class F2, class... Fs, typename std::enable_if<
      ! detail::is_tuple<ST>::value, int>::type = 0>
    decltype(auto) match(ST const& that, F1 && f1, F2 && f2, Fs &&... fs)
    {
#if ! defined YAFPL_X1
      return match(id<R>{}, that, overload(std::forward<F1>(f1), std::forward<F2>(f2), std::forward<Fs>(fs)...));
#else
      return match(that, overload(std::forward<F1>(f1), std::forward<F2>(f2), std::forward<Fs>(fs)...));
#endif
    }
    template <class R, class ST, class F1, class... Fs, typename std::enable_if<
      ! detail::is_tuple<ST>::value, int>::type = 0>
    decltype(auto) match(ST & that, F1 && f1, Fs &&... fs)
    {
#if ! defined YAFPL_X1
      return match(id<R>{}, that, overload(std::forward<F1>(f1), std::forward<Fs>(fs)...));
#else
      return match(that, overload(std::forward<F1>(f1), std::forward<Fs>(fs)...));
#endif
    }

    // explicit result type on a product of sum types
    template <class R, class... STs, class... Fs>
    decltype(auto) match(std::tuple<STs...> const& those, Fs &&... fcts)
    {
      return apply(
          [&](auto && ... args) -> decltype(auto)
          {
            return detail::apply_impl<R>(overload(std::forward<Fs>(fcts)...), std::forward<decltype(args)>(args)...);
          },
          those);
    }

    // result type deduced the nested typedef result_type of all functions
    template <class ST, class F1, class F2, class... Fs, typename std::enable_if<
      ! detail::is_tuple<ST>::value
      && detail::have_result_type_member<F1>::value, int>::type = 0>
    decltype(auto) match(ST const& that, F1 && f1, F2 && f2, Fs &&... fs)
    {
      using R = detail::deduced_result_type_t<std::decay_t<F1>, std::decay_t<F2>, std::decay_t<Fs>...>;
      return match<R>(that, std::forward<F1>(f1), std::forward<F2>(f2), std::forward<Fs>(fs)...);
    }
    template <class ST, class F1, class F2, class... Fs, typename std::enable_if<
      ! detail::is_tuple<ST>::value
      && detail::have_result_type_member<F1>::value, int>::type = 0>
    decltype(auto) match(ST& that, F1 && f1, F2 && f2, Fs &&... fs)
    {
      using R = detail::deduced_result_type_t<std::decay_t<F1>, std::decay_t<F2>, std::decay_t<Fs>...>;
      return match<R>(that, std::forward<F1>(f1), std::forward<F2>(f2), std::forward<Fs>(fs)...);
    }

    // result type deduced the nested typedef result_type of all functions on a product of sum types
    template <class... STs, class F, class... Fs, typename std::enable_if<
      detail::have_result_type_member<F>::value, int>::type = 0>
    decltype(auto) match(std::tuple<STs...> const& those, F && f, Fs &&... fs)
    {
      using R = detail::deduced_result_type_t<std::decay_t<F>, std::decay_t<Fs>...>;
      return match<R>(those, std::forward<F>(f), std::forward<Fs>(fs)...);
    }

  } // version
} // yafpl

#endif // header
