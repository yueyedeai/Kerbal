/**
 * @file       swap.hpp
 * @brief
 * @date       2020-06-17
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef KERBAL_ALGORITHM_SWAP_HPP
#define KERBAL_ALGORITHM_SWAP_HPP

#include <kerbal/compatibility/constexpr.hpp>
#include <kerbal/compatibility/move.hpp>
#include <kerbal/compatibility/noexcept.hpp>
#include <kerbal/iterator/iterator.hpp>

#include <cstddef>

#if __cplusplus >= 201103L
#	include <type_traits>
#endif


namespace kerbal
{

	namespace algorithm
	{
		template <typename Tp>
		KERBAL_CONSTEXPR14
		void swap(Tp & lhs, Tp & rhs)
							KERBAL_CONDITIONAL_NOEXCEPT(
									std::is_nothrow_move_constructible<Tp>::value &&
									std::is_nothrow_move_assignable<Tp>::value
							)
		;

		template <typename Tp, size_t N>
		KERBAL_CONSTEXPR14
		void swap(Tp (&lhs)[N], Tp (&rhs)[N]);


		template <typename Tp>
		KERBAL_CONSTEXPR14
		void swap(Tp & lhs, Tp & rhs)
							KERBAL_CONDITIONAL_NOEXCEPT(
									std::is_nothrow_move_constructible<Tp>::value &&
									std::is_nothrow_move_assignable<Tp>::value
							)
		{
			Tp t(kerbal::compatibility::to_xvalue(lhs));
			lhs = kerbal::compatibility::to_xvalue(rhs);
			rhs = kerbal::compatibility::to_xvalue(t);
		}

		template <typename ForwardIterator1, typename ForwardIterator2>
		KERBAL_CONSTEXPR14
		void iter_swap(ForwardIterator1 lhs, ForwardIterator2 rhs)
							KERBAL_CONDITIONAL_NOEXCEPT(
									noexcept(kerbal::algorithm::swap(*lhs, *rhs))
							)
		{
			kerbal::algorithm::swap(*lhs, *rhs);
		}

		template <typename ForwardIterator1, typename ForwardIterator2>
		KERBAL_CONSTEXPR14
		ForwardIterator2
		__range_swap(ForwardIterator1 a_first, ForwardIterator1 a_last, ForwardIterator2 b_first,
						std::forward_iterator_tag)
											KERBAL_CONDITIONAL_NOEXCEPT(
													noexcept(static_cast<bool>(a_first != a_last)) &&
													noexcept(kerbal::algorithm::iter_swap(a_first, b_first)) &&
													noexcept(++a_first) &&
													noexcept(++b_first)
											)
		{
			while (a_first != a_last) {
				kerbal::algorithm::iter_swap(a_first, b_first);
				++a_first;
				++b_first;
			}
			return b_first;
		}

		template <typename RandomAccessIterator1, typename ForwardIterator2>
		KERBAL_CONSTEXPR14
		ForwardIterator2
		__range_swap(RandomAccessIterator1 a_first, RandomAccessIterator1 a_last, ForwardIterator2 b_first,
						std::random_access_iterator_tag)
		{
			typedef RandomAccessIterator1 iterator;
			typedef typename kerbal::iterator::iterator_traits<iterator>::difference_type difference_type;

#	define EACH() do {\
				kerbal::algorithm::iter_swap(a_first, b_first);\
				++a_first;\
				++b_first;\
			} while (false)

			for (difference_type trip_count(kerbal::iterator::distance(a_first, a_last) >> 2); trip_count > 0; --trip_count) {
				EACH();
				EACH();
				EACH();
				EACH();
			}
			difference_type remain(kerbal::iterator::distance(a_first, a_last));
			if (remain == 3) {
				EACH();
			}
			if (remain >= 2) {
				EACH();
			}
			if (remain >= 1) {
				EACH();
			}

#	undef EACH

			return b_first;
		}

		template <typename ForwardIterator1, typename ForwardIterator2>
		KERBAL_CONSTEXPR14
		ForwardIterator2
		range_swap(ForwardIterator1 a_first, ForwardIterator1 a_last, ForwardIterator2 b_first)
		{
			return kerbal::algorithm::__range_swap(a_first, a_last, b_first,
									kerbal::iterator::iterator_category(a_first));
		}

		template <typename Tp, size_t N>
		KERBAL_CONSTEXPR14
		void swap(Tp (&lhs)[N], Tp (&rhs)[N])
		{
			kerbal::algorithm::range_swap(lhs + 0, lhs + N, rhs + 0);
		}

	} // namespace algorithm

} // namespace kerbal

#endif // KERBAL_ALGORITHM_SWAP_HPP
