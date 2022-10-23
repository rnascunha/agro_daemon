#include "include/csv/fwriter.hpp"

namespace CSV{

fwriter::fwriter(std::string const& filename, char sep /* = ',' */, const char* nl /* = "\r\n" */)
	: writer(nullptr, sep, nl), ofs_{filename, std::ios::out}
{
	this->streambuffer(ofs_.rdbuf());
}

bool fwriter::is_open() const noexcept
{
	return ofs_.is_open();
}

void fwriter::close() noexcept
{
	ofs_.close();
}

}//CSV
