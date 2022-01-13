#ifndef AGRO_DAEMON_LIBS_CSV_STRING_HPP__
#define AGRO_DAEMON_LIBS_CSV_STRING_HPP__

#include "writer.hpp"
#include <sstream>

namespace CSV{

class swriter : public writer{
	public:
		swriter(char sep = ',', const char* nl = "\r\n");

		std::string str() const noexcept;
	private:
		std::ostringstream sfs_;
};

}//CSV

#endif /* AGRO_DAEMON_LIBS_CSV_STRING_HPP__ */
