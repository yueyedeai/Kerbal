/**
 * @file       throw_this_exception.hpp
 * @brief      
 * @date       2019-7-29
 * @author     peter
 * @copyright
 *      peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef KERBAL_UTILITY_THROW_THIS_EXCEPTION_HPP_
#define KERBAL_UTILITY_THROW_THIS_EXCEPTION_HPP_

#include <kerbal/compatibility/constexpr.hpp>
#include <kerbal/compatibility/noexcept.hpp>
#include <utility>

#if !__cpp_exceptions
#include <cstdlib>
#endif

namespace kerbal
{
	namespace utility
	{
		template <typename Exception>
		class throw_this_exception_helper
		{
			public:

#	if __cpp_exceptions

#		if __cplusplus >= 201103L

					template <typename ... Args>
					[[noreturn]]
					static void throw_this_exception(Args&& ... args)
					{
						throw Exception(std::forward<Args>(args)...);
					}

#		else

					static void throw_this_exception()
					{
						throw Exception();
					}

					template <typename Tp0>
					static void throw_this_exception(const Tp0 & val0)
					{
						throw Exception(val0);
					}

					template <typename Tp0, typename Tp1>
					static void throw_this_exception(const Tp0 & val0, const Tp1 & val1)
					{
						throw Exception(val0, val1);
					}

					template <typename Tp0, typename Tp1, typename Tp2>
					static void throw_this_exception(const Tp0 & val0, const Tp1 & val1, const Tp2 & val2)
					{
						throw Exception(val0, val1, val2);
					}

#		endif

#	else // no exception

#		if __cplusplus >= 201103L

					template <typename ... Args>
					[[noreturn]]
					static void throw_this_exception(Args&& ...) noexcept
					{
						std::abort();
					}

#		else

					static void throw_this_exception() KERBAL_NOEXCEPT
					{
						std::abort();
					}

					template <typename Tp0>
					static void throw_this_exception(const Tp0 &) KERBAL_NOEXCEPT
					{
						std::abort();
					}

					template <typename Tp0, typename Tp1>
					static void throw_this_exception(const Tp0 &, const Tp1 &) KERBAL_NOEXCEPT
					{
						std::abort();
					}

					template <typename Tp0, typename Tp1, typename Tp2>
					static void throw_this_exception(const Tp0 &, const Tp1 &, const Tp2 &) KERBAL_NOEXCEPT
					{
						std::abort();
					}

#		endif

#	endif

		};

	} // namespace utility

} // namespace kerbal

#endif /* KERBAL_UTILITY_THROW_THIS_EXCEPTION_HPP_ */
