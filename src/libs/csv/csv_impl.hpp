#ifndef AGRO_DAEMON_LIB_CSV_WRITE_IMPL_HPP__
#define AGRO_DAEMON_LIB_CSV_WRITE_IMPL_HPP__

#include <type_traits>
#include "csv.hpp"

namespace CSV{

template<typename Value>
writer& writer::column_arthimectic(Value&& v) noexcept
{
	static_assert(!std::is_arithmetic<Value>::value, "Value must be a float or integer");

	if(has_col_)
	{
		os_ << sep_;
	}
	else
	{
		has_col_ = true;
	}
	os_ << v;

	return *this;
}

template<typename Value>
writer& writer::operator<<(Value&& v) noexcept
{
	return column(v);
}

template<typename ...Args>
writer& writer::column(Args&&... args) noexcept
{
	column_impl(std::forward<Args>(args)...);
	return *this;
}

template<typename ...Args>
writer& writer::row(Args&&... args) noexcept
{
	row_impl(std::forward<Args>(args)...);
	return *this;
}

template<typename Arg>
void writer::column_impl(Arg&& arg) noexcept
{
	column(arg);
}

template<typename Arg, typename ...Args>
void writer::column_impl(Arg&& arg, Args&& ...args) noexcept
{
	column(arg);
	column_impl(std::forward<Args>(args)...);
}

template<typename Arg>
void writer::row_impl(Arg&& arg) noexcept
{
	column(arg);
	nl();
}

template<typename Arg, typename ...Args>
void writer::row_impl(Arg&& arg, Args&& ...args) noexcept
{
	column(arg);
	row_impl(std::forward<Args>(args)...);
}

}//CSV

#endif /* AGRO_DAEMON_LIB_CSV_WRITE_IMPL_HPP__ */
