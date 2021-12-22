#include "fcsv.hpp"

namespace CSV{

fwriter::fwriter(std::string const& filename, char sep /* = ',' */, const char* nl /* = "\r\n" */)
	: writer(ofs_.rdbuf(), sep, nl), ofs_{filename, std::ios::out}{}

bool fwriter::is_open() const noexcept
{
	return ofs_.is_open();
}

void fwriter::close() noexcept
{
	ofs_.close();
}

}//CSV
