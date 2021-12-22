#include "scsv.hpp"

namespace CSV{

swriter::swriter(char sep /* = ',' */, const char* nl /* = "\r\n" */)
	: writer(sfs_.rdbuf(), sep, nl){}

std::string swriter::str() const noexcept
{
	return sfs_.str();
}

}//CSV
