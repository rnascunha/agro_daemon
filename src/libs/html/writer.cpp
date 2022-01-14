#include "include/html/writer.hpp"

namespace HTML{

struct escape_char{
	char		to_escape;
	const char*	to_what;
};

static constexpr const escape_char escape_chars[] = {
		{'&', "&amp"},
		{'<', "&lt"},
		{'>', "&gt"},
		{'"', "&quot"},
		{'\'', "&apos"}
};
static constexpr const std::size_t size_arr = std::extent<decltype(escape_chars)>::value;

writer::writer(std::streambuf* sb)
	: os_{sb}{}

writer& writer::init(const char* header /* = default_header */) noexcept
{
	os_ << "<html><head>" << header << "</head><body><table>";
	return *this;
}

writer& writer::header(char val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(int val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(long val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(unsigned val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(float val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(double val) noexcept
{
	return column_val_impl(val, "th");
}

writer& writer::header(std::string const& str) noexcept
{
	return column_str_impl(str, "th");
}

writer& writer::header(std::string&& str) noexcept
{
	return column_str_impl(static_cast<std::string const&>(str), "th");
}

writer& writer::header(const char* str) noexcept
{
	return column_str_impl(std::string{str}, "th");
}

writer& writer::header(io) noexcept
{
	return nl();
}

writer& writer::column(char val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(int val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(long val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(unsigned val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(float val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(double val) noexcept
{
	return column_val_impl(val);
}

writer& writer::column(std::string const& str) noexcept
{
	return column_str_impl(str);
}

writer& writer::column(std::string&& str) noexcept
{
	return column_str_impl(str);
}

writer& writer::column(const char* str) noexcept
{
	return column_str_impl(std::string{str});
}

writer& writer::column(io) noexcept
{
	return nl();
}

writer& writer::nl() noexcept
{
	has_col_ = false;
	os_ << "</tr>";
	return *this;
}

void writer::end() noexcept
{
	if(has_col_)
	{
		nl();
	}
	os_ << "</table></body></html>";
}

writer& writer::column_str_impl(std::string const& v, const char* tag /* = td */) noexcept
{
	if(!has_col_)
	{
		has_col_ = true;
		os_ << "<tr>";
	}

	os_ << "<" << tag << ">";
	for(char const& c : v)
	{
		bool flag = true;
		for(std::size_t i = 0; i < size_arr; i++)
		{
			if(escape_chars[i].to_escape == c)
			{
				os_ << escape_chars[i].to_what;
				flag = false;
				break;
			}
		}
		if(flag)
		{
			os_ << c;
		}
	}
	os_ << "</" << tag << ">";
	return *this;
}

}//HTML
