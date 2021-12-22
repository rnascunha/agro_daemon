#ifndef AGRO_DAEMON_LIBS_CSV_FILE_HPP__
#define AGRO_DAEMON_LIBS_CSV_FILE_HPP__

#include "csv.hpp"
#include <fstream>

namespace CSV{

class fwriter : public writer{
	public:
		fwriter(std::string const& filename, char sep = ',', const char* nl = "\r\n");

		bool is_open() const noexcept;
		void close() noexcept;
	private:
		std::ofstream ofs_;
};

}//CSV

#endif /* AGRO_DAEMON_LIBS_CSV_FILE_HPP__ */
