#include "include/html/swriter.hpp"

namespace HTML{

swriter::swriter()
	: writer(nullptr)
{
	this->streambuffer(sfs_.rdbuf());
}

std::string swriter::str() const noexcept
{
	return sfs_.str();
}

}//CSV
