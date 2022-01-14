#ifndef AGRO_DAEMON_LIBS_HTML_WRITE_FILE_HPP__
#define AGRO_DAEMON_LIBS_HTML_WRITE_FILE_HPP__

#include "writer.hpp"
#include <fstream>

namespace HTML{

class fwriter : public writer{
	public:
		fwriter(std::string const& filename);

		bool is_open() const noexcept;
		void close() noexcept;
	private:
		std::ofstream ofs_;
};

}//HTML

#endif /* AGRO_DAEMON_LIBS_HTML_WRITE_FILE_HPP__ */
