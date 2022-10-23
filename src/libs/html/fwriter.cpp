#include "include/html/fwriter.hpp"

namespace HTML{

fwriter::fwriter(std::string const& filename)
	: writer(nullptr), ofs_{filename, std::ios::out}
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

}//HTML
