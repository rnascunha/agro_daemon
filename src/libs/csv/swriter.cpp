#include "include/csv/swriter.hpp"

namespace CSV{

swriter::swriter(char sep /* = ',' */, const char* nl /* = "\r\n" */)
	: writer(nullptr, sep, nl)
{
	this->streambuffer(sfs_.rdbuf());
}

std::string swriter::str() const noexcept
{
	return sfs_.str();
}

}//CSV
