// Copyright (C) 2004 Arkadiy Vertleyb
// Use, modification and distribution is subject to the Boost Software
// License, Version 1.0. (http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_TYPEOF_INT_ENCODING_HPP_INCLUDED
#define BOOST_TYPEOF_INT_ENCODING_HPP_INCLUDED

#include <boost/mpl/if.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/size_t.hpp>

namespace boost{namespace type_of{

    template<class T> struct get_unsigned
    {
        typedef T type;
    };
    template<> struct get_unsigned<signed char>
    {
        typedef unsigned char type;
    };
    template<> struct get_unsigned<char>
    {
        typedef unsigned char type;
    };
    template<> struct get_unsigned<short>
    {
        typedef unsigned short type;
    };
    template<> struct get_unsigned<int>
    {
        typedef unsigned int type;
    };
    template<> struct get_unsigned<long>
    {
        typedef unsigned long type;
    };

    //////////////////////////

    template<size_t n, bool Overflow> 
    struct pack
    {
        static const size_t value = 
            (n + 1) * 2 + (Overflow ? 1 : 0);
    };

    template<size_t m> 
    struct unpack
    {
        static const size_t value =
            (m / 2) - 1;

        static const bool overflow = 
            (m % 2 == 1);
    };

    ////////////////////////////////

    template<class V, size_t n, bool overflow = (n >= 0x3fffffff)>
    struct encode_size_t : BOOST_TYPEOF_PUSH_BACK<
        V, 
        boost::mpl::size_t<pack<n, false>::value> 
    >
    {};

    template<class V, size_t n>
    struct encode_size_t<V, n, true> : BOOST_TYPEOF_PUSH_BACK<typename BOOST_TYPEOF_PUSH_BACK<
        V,
        boost::mpl::size_t<pack<n % 0x3ffffffe, true>::value> >::type,
        boost::mpl::size_t<n / 0x3ffffffe>
    >
    {};

    template<class V, class T, T n>
    struct encode_integral : encode_size_t< V, (typename get_unsigned<T>::type)n > 
    {};

    ///////////////////////////

    template<size_t n, class Iter, bool overflow> 
    struct decode_size_t;

    template<size_t n, class Iter> 
    struct decode_size_t<n, Iter, false>
    {
        static const size_t value = n;
        typedef Iter iter;
    };

    template<size_t n, class Iter> 
    struct decode_size_t<n, Iter, true>
    {
        static const size_t m = boost::mpl::deref<Iter>::type::value;

        static const size_t value = m * 0x3ffffffe + n;
        typedef typename boost::mpl::next<Iter>::type iter;
    };

    template<class T, class Iter>
    struct decode_integral
    {
        static const size_t m = 
            boost::mpl::deref<Iter>::type::value;

        static const size_t n = 
            unpack<m>::value;

        static const bool overflow =
            unpack<m>::overflow;

        typedef typename boost::mpl::next<Iter>::type nextpos;
        
        static const T value = 
            decode_size_t<n, nextpos, overflow>::value;

        typedef typename decode_size_t<n, nextpos, overflow>::iter iter;
    };

}}//namespace 

#endif//BOOST_TYPEOF_INT_ENCODING_HPP_INCLUDED
