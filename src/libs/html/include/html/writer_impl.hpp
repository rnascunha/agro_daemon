#ifndef AGRO_DAEMON_LIB_HTML_WRITE_IMPL_HPP__
#define AGRO_DAEMON_LIB_HTML_WRITE_IMPL_HPP__

#include <type_traits>
#include "writer.hpp"

namespace HTML{

template<typename Value>
writer& writer::column_val_impl(Value&& v, const char* tag /* = td */) noexcept
{
	if constexpr(std::is_same<std::string, std::remove_cv_t<std::remove_reference_t<Value>>>::value)
	{
		return column_str_impl(std::forward<Value>(v), tag);
	}
	else
	{
		if(!has_col_)
		{
			has_col_ = true;
			os_ << "<tr>";
		}
		os_ << "<" << tag << ">" << v << "</" << tag << ">";
		return *this;
	}
}

template<typename Value>
writer& writer::operator<<(Value&& v) noexcept
{
	return column(v);
}

template<typename ...Args>
writer& writer::column(Args&&... args) noexcept
{
	if constexpr(sizeof...(args) == 1)
	{
		return column_val_impl(args...);
	}
	else
	{
		column_impl(std::forward<Args>(args)...);
		return *this;
	}
}

template<typename ...Args>
writer& writer::header(Args&&... args) noexcept
{
	if constexpr(sizeof...(args) == 1)
	{
		return column_val_impl(args..., "th");
	}
	else
	{
		header_impl(std::forward<Args>(args)...);
		return *this;
	}
}

template<typename ...Args>
writer& writer::row(Args&&... args) noexcept
{
	row_impl(std::forward<Args>(args)...);
	return *this;
}

template<typename ...Args>
writer& writer::hrow(Args&&... args) noexcept
{
	hrow_impl(std::forward<Args>(args)...);
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
void writer::header_impl(Arg&& arg) noexcept
{
	header(arg);
}

template<typename Arg, typename ...Args>
void writer::header_impl(Arg&& arg, Args&& ...args) noexcept
{
	header(arg);
	header_impl(std::forward<Args>(args)...);
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

template<typename Arg>
void writer::hrow_impl(Arg&& arg) noexcept
{
	header(arg);
	nl();
}

template<typename Arg, typename ...Args>
void writer::hrow_impl(Arg&& arg, Args&& ...args) noexcept
{
	header(arg);
	hrow_impl(std::forward<Args>(args)...);
}

}//HTML

#endif /* AGRO_DAEMON_LIB_HTML_WRITE_IMPL_HPP__ */
