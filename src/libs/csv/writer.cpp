#include "include/csv/writer.hpp"

namespace CSV{

writer::writer(std::streambuf* sb,
		char sep /* = ',' */,
		const char* nl /* = "\r\n" */) // @suppress("Member declaration not found")
	: os_{sb}, sep_{sep}, nl_{nl}{}

writer& writer::column(char val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(int val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(long val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(unsigned val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(float val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(double val) noexcept
{
	return column_arthimectic(val);
}

writer& writer::column(std::string const& str) noexcept
{
	return column_str(str);
}

writer& writer::column(std::string&& str) noexcept
{
	return column(static_cast<std::string const&>(str));
}

writer& writer::column(const char* str) noexcept
{
	return column(std::string{str});
}

writer& writer::column(io) noexcept
{
	return nl();
}

writer& writer::nl() noexcept
{
	has_col_ = false;
	os_ << nl_;
	return *this;
}

writer& writer::column_str(std::string const& str) noexcept
{
	bool use_dquote = false;
	for(char const& c : str)
	{
		if(c == sep_ || c == dquote)
		{
			use_dquote = true;
			break;
		}
		const char* nl_c = nl_;
		while(*nl_c != '\0')
		{
			if(*nl_c == c)
			{
				use_dquote = true;
				break;
			}
			nl_c++;
		}
		if(use_dquote)
		{
			break;
		}
	}

	if(has_col_)
	{
		os_ << sep_;
	}
	else
	{
		has_col_ = true;
	}

	if(use_dquote)
	{
		os_ << dquote;
	}

	for(char const& c : str)
	{
		if(c == dquote)
		{
			os_ << dquote;
		}

		os_ << c;
	}

	if(use_dquote)
	{
		os_ << dquote;
	}

	return *this;
}

}//CSV
