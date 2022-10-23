#ifndef AGRO_DAEMON_LIB_CSV_WRITE_HPP__
#define AGRO_DAEMON_LIB_CSV_WRITE_HPP__

#include <ostream>
#include <string>

/**
 * https://datatracker.ietf.org/doc/html/rfc4180
 */

namespace CSV{

enum class io{
	break_line = 0
};

class writer{
	public:
		static constexpr const char dquote = '"';

		writer(std::streambuf* sb, char sep = ',', const char* nl = "\r\n");

		writer& column(char) noexcept;
		writer& column(int) noexcept;
		writer& column(long) noexcept;
		writer& column(unsigned) noexcept;
		writer& column(float) noexcept;
		writer& column(double) noexcept;

		writer& column(std::string const&) noexcept;
		writer& column(std::string&&) noexcept;

		writer& column(const char*) noexcept;
		writer& column(io) noexcept;

		writer& nl() noexcept;

		template<typename ...Args>
		writer& column(Args&&...) noexcept;

		template<typename ...Args>
		writer& row(Args&&...) noexcept;

		template<typename Value>
		writer& operator<<(Value&&) noexcept;
	protected:
		void streambuffer(std::streambuf* sb) noexcept;
	private:
		std::ostream	os_;
		char			sep_;
		const char*		nl_;
		bool			has_col_ = false;

		template<typename Value>
		writer& column_val_impl(Value&&) noexcept;

		template<typename Arg>
		writer& column_arthimectic(Arg&&) noexcept;
		writer& column_str(std::string const& str) noexcept;

		template<typename Arg>
		void column_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void column_impl(Arg&& arg, Args&& ...args) noexcept;

		template<typename Arg>
		void row_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void row_impl(Arg&& arg, Args&& ...args) noexcept;
};

}//CSV

#include "writer_impl.hpp"

#endif /* AGRO_DAEMON_LIB_CSV_WRITE_HPP__ */
