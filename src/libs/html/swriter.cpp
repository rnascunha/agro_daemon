#include "include/html/swriter.hpp"

namespace HTML{

swriter::swriter()
	: writer(sfs_.rdbuf()){}

std::string swriter::str() const noexcept
{
	return sfs_.str();
}

}//CSV
