/**
 * @file       member_compress_helper.hpp
 * @brief
 * @date       2020-09-20
 * @author     Peter
 * @copyright
 *      Peter of [ThinkSpirit Laboratory](http://thinkspirit.org/)
 *   of [Nanjing University of Information Science & Technology](http://www.nuist.edu.cn/)
 *   all rights reserved
 */

#ifndef KERBAL_UTILITY_MEMBER_COMPRESS_HELPER_HPP
#define KERBAL_UTILITY_MEMBER_COMPRESS_HELPER_HPP

#include <kerbal/compatibility/constexpr.hpp>
#include <kerbal/compatibility/method_overload_tag.hpp>
#include <kerbal/compatibility/move.hpp>
#include <kerbal/compatibility/noexcept.hpp>
#include <kerbal/type_traits/can_be_empty_base.hpp>
#include <kerbal/type_traits/conditional.hpp>
#include <kerbal/type_traits/cv_deduction.hpp>
#include <kerbal/type_traits/reference_deduction.hpp>
#include <kerbal/utility/declval.hpp>
#include <kerbal/utility/in_place.hpp>
#include <kerbal/utility/noncopyable.hpp>

#if __cplusplus >= 201103L
#	include <kerbal/utility/forward.hpp>
#endif

#include <cstddef>

namespace kerbal
{

	namespace utility
	{

		template <typename T, size_t I = 0>
		class member_compress_helper;

		namespace detail
		{

			template <typename T, bool = kerbal::type_traits::can_be_empty_base<T>::value>
			class member_compress_helper_impl;

			template <typename T>
			class member_compress_helper_impl<T, false>
			{
				private:
					T M_member;

				public:
					typedef kerbal::type_traits::false_type IS_COMPRESSED;

					typedef T																	value_type;
					typedef typename kerbal::type_traits::add_lvalue_reference<T>::type			reference;
					typedef typename kerbal::type_traits::add_const_lvalue_reference<T>::type	const_reference;

#			if __cplusplus >= 201103L
					typedef typename kerbal::type_traits::add_rvalue_reference<T>::type			rvalue_reference;
					typedef typename kerbal::type_traits::add_const_rvalue_reference<T>::type	const_rvalue_reference;
#			endif

				public:

#			if __cplusplus >= 201103L

					template <typename ... Args>
					KERBAL_CONSTEXPR
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, Args&& ... args)
									KERBAL_CONDITIONAL_NOEXCEPT(
											(std::is_nothrow_constructible<T, Args...>::value)
									) :
							M_member(kerbal::utility::forward<Args>(args)...)
					{
					}

#			else

					explicit member_compress_helper_impl(kerbal::utility::in_place_t):
							M_member()
					{
					}

					template <typename Arg0>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0):
							M_member(arg0)
					{
					}

					template <typename Arg0, typename Arg1>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0, const Arg1 & arg1):
							M_member(arg0, arg1)
					{
					}

					template <typename Arg0, typename Arg1, typename Arg2>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0, const Arg1 & arg1, const Arg2 & arg2):
							M_member(arg0, arg1, arg2)
					{
					}

#			endif

					template <typename U, size_t J>
					KERBAL_CONSTEXPR14
					explicit member_compress_helper_impl(const kerbal::utility::member_compress_helper<U, J> & src)
									KERBAL_CONDITIONAL_NOEXCEPT((
											std::is_nothrow_constructible<
													T,
													typename kerbal::utility::member_compress_helper<U, J>::const_reference
											>::value
									)) :
							M_member(src.member())
					{
					}

#			if __cplusplus >= 201103L

					template <typename U, size_t J>
					KERBAL_CONSTEXPR14
					explicit member_compress_helper_impl(kerbal::utility::member_compress_helper<U, J> && src)
								KERBAL_CONDITIONAL_NOEXCEPT((
										std::is_nothrow_constructible<
												T,
												typename kerbal::utility::member_compress_helper<U, J>::rvalue_reference
										>::value
								)) :
							M_member(kerbal::compatibility::move(src.member()))
					{
					}

#			endif

					KERBAL_CONSTEXPR14
					reference member() KERBAL_REFERENCE_OVERLOAD_TAG KERBAL_NOEXCEPT
					{
						return this->M_member;
					}

					KERBAL_CONSTEXPR14
					const_reference member() KERBAL_CONST_REFERENCE_OVERLOAD_TAG KERBAL_NOEXCEPT
					{
						return this->M_member;
					}

#			if __cplusplus >= 201103L

					KERBAL_CONSTEXPR14
					rvalue_reference member() && KERBAL_NOEXCEPT
					{
						return kerbal::compatibility::move(this->M_member);
					}

					KERBAL_CONSTEXPR14
					const_rvalue_reference member() const && KERBAL_NOEXCEPT
					{
						return kerbal::compatibility::move(this->M_member);
					}

#			endif

			};

			struct member_compress_allow_copy_assignable
			{
			};

			template <typename T>
			class member_compress_helper_impl<T, true>:
					private kerbal::type_traits::remove_cv<T>::type,
					private kerbal::type_traits::conditional<
							kerbal::type_traits::is_const<T>::value,
							kerbal::utility::noncopyassignable,
							member_compress_allow_copy_assignable
					>::type
			{
				private:
					typedef typename kerbal::type_traits::remove_cv<T>::type super;

				public:
					typedef kerbal::type_traits::true_type IS_COMPRESSED;

					typedef T																	value_type;
					typedef typename kerbal::type_traits::add_lvalue_reference<T>::type			reference;
					typedef typename kerbal::type_traits::add_const_lvalue_reference<T>::type	const_reference;

#			if __cplusplus >= 201103L
					typedef typename kerbal::type_traits::add_rvalue_reference<T>::type			rvalue_reference;
					typedef typename kerbal::type_traits::add_const_rvalue_reference<T>::type	const_rvalue_reference;
#			endif

				public:

#			if __cplusplus >= 201103L

					template <typename ... Args>
					KERBAL_CONSTEXPR
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, Args&& ... args)
									KERBAL_CONDITIONAL_NOEXCEPT(
											(std::is_nothrow_constructible<super, Args...>::value)
									) :
							super(kerbal::utility::forward<Args>(args)...)
					{
					}

#			else

					explicit member_compress_helper_impl(kerbal::utility::in_place_t):
							super()
					{
					}

					template <typename Arg0>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0):
							super(arg0)
					{
					}

					template <typename Arg0, typename Arg1>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0, const Arg1 & arg1):
							super(arg0, arg1)
					{
					}

					template <typename Arg0, typename Arg1, typename Arg2>
					explicit member_compress_helper_impl(kerbal::utility::in_place_t, const Arg0 & arg0, const Arg1 & arg1, const Arg2 & arg2):
							super(arg0, arg1, arg2)
					{
					}

#			endif

					template <typename U, size_t J>
					KERBAL_CONSTEXPR14
					explicit member_compress_helper_impl(const kerbal::utility::member_compress_helper<U, J> & src)
									KERBAL_CONDITIONAL_NOEXCEPT((
											std::is_nothrow_constructible<
													T,
													typename kerbal::utility::member_compress_helper<U, J>::const_reference
											>::value
									)) :
							super(src.member())
					{
					}

#			if __cplusplus >= 201103L

					template <typename U, size_t J>
					KERBAL_CONSTEXPR14
					explicit member_compress_helper_impl(kerbal::utility::member_compress_helper<U, J> && src)
									KERBAL_CONDITIONAL_NOEXCEPT((
											std::is_nothrow_constructible<
													T,
													typename kerbal::utility::member_compress_helper<U, J>::rvalue_reference
											>::value
									)) :
							super(kerbal::compatibility::move(src.member()))
					{
					}

#			endif

					KERBAL_CONSTEXPR14
					reference member() KERBAL_REFERENCE_OVERLOAD_TAG KERBAL_NOEXCEPT
					{
						return static_cast<reference>(*this);
					}

					KERBAL_CONSTEXPR14
					const_reference member() KERBAL_CONST_REFERENCE_OVERLOAD_TAG KERBAL_NOEXCEPT
					{
						return static_cast<const_reference>(*this);
					}

#			if __cplusplus >= 201103L

					KERBAL_CONSTEXPR14
					rvalue_reference member() && KERBAL_NOEXCEPT
					{
						return static_cast<rvalue_reference>(*this);
					}

					KERBAL_CONSTEXPR14
					const_rvalue_reference member() const && KERBAL_NOEXCEPT
					{
						return static_cast<const_rvalue_reference>(*this);
					}

#			endif

			};

		} // namespace detail

		template <typename T, size_t I>
		class member_compress_helper: public detail::member_compress_helper_impl<T>
		{
			private:
				typedef kerbal::utility::detail::member_compress_helper_impl<T> super;

			public:
				typedef typename super::value_type					value_type;
				typedef typename super::reference					reference;
				typedef typename super::const_reference				const_reference;

#		if __cplusplus >= 201103L
				typedef typename super::rvalue_reference			rvalue_reference;
				typedef typename super::const_rvalue_reference		const_rvalue_reference;
#		endif

			public:

#		if __cplusplus < 201103L

				explicit member_compress_helper(kerbal::utility::in_place_t in_place):
						super(in_place)
				{
				}

				template <typename Arg0>
				explicit member_compress_helper(kerbal::utility::in_place_t in_place, const Arg0 & arg0):
						super(in_place, arg0)
				{
				}

				template <typename Arg0, typename Arg1>
				explicit member_compress_helper(kerbal::utility::in_place_t in_place, const Arg0 & arg0, const Arg1 & arg1):
						super(in_place, arg0, arg1)
				{
				}

				template <typename Arg0, typename Arg1, typename Arg2>
				explicit member_compress_helper(kerbal::utility::in_place_t in_place, const Arg0 & arg0, const Arg1 & arg1, const Arg2 & arg2):
						super(in_place, arg0, arg1, arg2)
				{
				}

				template <typename U, size_t J>
				explicit member_compress_helper(const member_compress_helper<U, J> & arg):
						super(arg)
				{
				}

#		else

				using super::super;

				member_compress_helper(const member_compress_helper<T, I> & arg) noexcept = default;
				member_compress_helper(member_compress_helper<T, I> && arg) noexcept = default;

#		endif


#		if __cplusplus < 201103L

				template <typename U>
				void assign(const U & arg)
				{
					this->member() = arg;
				}

#		else

				template <typename U>
				KERBAL_CONSTEXPR14
				void assign(U && arg)
						KERBAL_CONDITIONAL_NOEXCEPT(
								noexcept(kerbal::utility::declthis<member_compress_helper>()->member() = kerbal::utility::forward<U>(arg))
						)
				{
					this->member() = kerbal::utility::forward<U>(arg);
				}

#		endif


				template <typename U, size_t J>
				KERBAL_CONSTEXPR14
				member_compress_helper& operator=(const member_compress_helper<U, J> & arg)
						KERBAL_CONDITIONAL_NOEXCEPT(
								noexcept(kerbal::utility::declthis<member_compress_helper>()->member() = arg.member())
						)
				{
					this->member() = arg.member();
					return *this;
				}

#		if __cplusplus >= 201103L

				member_compress_helper& operator=(const member_compress_helper & arg) noexcept = default;
				member_compress_helper& operator=(member_compress_helper && arg) noexcept = default;

				template <typename U, size_t J>
				KERBAL_CONSTEXPR14
				member_compress_helper& operator=(member_compress_helper<U, J> && arg)
						KERBAL_CONDITIONAL_NOEXCEPT(
								noexcept(kerbal::utility::declthis<member_compress_helper>()->member() = kerbal::compatibility::move(arg.member()))
						)
				{
					this->member() = kerbal::compatibility::move(arg.member());
					return *this;
				}

#		endif

		};

		template <typename T, size_t I>
		class member_compress_helper<T&, I>
		{
			public:
				typedef kerbal::type_traits::false_type IS_COMPRESSED;

				typedef T&																	value_type;
				typedef typename kerbal::type_traits::add_lvalue_reference<T&>::type		reference;
				typedef typename kerbal::type_traits::add_const_lvalue_reference<T&>::type	const_reference;

#		if __cplusplus >= 201103L
				typedef typename kerbal::type_traits::add_rvalue_reference<T&>::type		rvalue_reference;
				typedef typename kerbal::type_traits::add_const_rvalue_reference<T&>::type	const_rvalue_reference;
#		endif

			private:
				T & M_member;

			public:

				KERBAL_CONSTEXPR14
				explicit member_compress_helper(kerbal::utility::in_place_t, reference arg0) KERBAL_NOEXCEPT:
						M_member(arg0)
				{
				}

				template <size_t J>
				KERBAL_CONSTEXPR14
				explicit member_compress_helper(const member_compress_helper<T, J> & arg) KERBAL_NOEXCEPT:
						M_member(arg.member())
				{
				}

				template <size_t J>
				KERBAL_CONSTEXPR14
				explicit member_compress_helper(const member_compress_helper<T&, J> & arg) KERBAL_NOEXCEPT:
						M_member(arg.member())
				{
				}

				KERBAL_CONSTEXPR14
				void assign(T & arg)
				{
					this->member() = arg;
				}

				KERBAL_CONSTEXPR14
				member_compress_helper& operator=(const member_compress_helper & arg)
						KERBAL_CONDITIONAL_NOEXCEPT(
								noexcept(kerbal::utility::declthis<member_compress_helper>()->member() = arg.member())
						)
				{
					this->member() = arg.member();
					return *this;
				}

				template <typename U, size_t J>
				KERBAL_CONSTEXPR14
				member_compress_helper& operator=(const member_compress_helper<U, J> & arg)
						KERBAL_CONDITIONAL_NOEXCEPT(
								noexcept(kerbal::utility::declthis<member_compress_helper>()->member() = arg.member())
						)
				{
					this->member() = arg.member();
					return *this;
				}

				KERBAL_CONSTEXPR14
				reference member() const KERBAL_NOEXCEPT
				{
					return this->M_member;
				}

		};

	} // namespace utility

} // namespace kerbal

#endif // KERBAL_UTILITY_MEMBER_COMPRESS_HELPER_HPP
