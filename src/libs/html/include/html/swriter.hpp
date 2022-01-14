#ifndef AGRO_DAEMON_LIBS_HTML_WRITER_STRING_HPP__
#define AGRO_DAEMON_LIBS_HTML_WRITER_STRING_HPP__

#include "writer.hpp"
#include <sstream>

namespace HTML{

class swriter : public writer{
	public:
		swriter();

		std::string str() const noexcept;
	private:
		std::ostringstream sfs_;
};

}//CSV

#endif /* AGRO_DAEMON_LIBS_HTML_WRITER_STRING_HPP__ */
