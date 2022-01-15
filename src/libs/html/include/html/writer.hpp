#ifndef AGRO_DAEMON_LIB_HTML_WRITE_HPP__
#define AGRO_DAEMON_LIB_HTML_WRITE_HPP__

#include <ostream>
#include <string>

namespace HTML{

enum class io{
	break_line = 0
};

class writer{
	public:
		static constexpr const char* default_header =
		"<style>table,tr,td,th{border:1px solid black;border-collapse:collapse;text-align:center;padding:5px;}"
		"tr:nth-child(2n-1){background-color:lightgrey;}"
		"tr:hover{background-color:antiquewhite;}"
		"th{background-color:yellow;}</style>";

		writer(std::streambuf* sb);

		writer& init(const char* header = default_header) noexcept;

		writer& header(char) noexcept;
		writer& header(int) noexcept;
		writer& header(long) noexcept;
		writer& header(unsigned) noexcept;
		writer& header(float) noexcept;
		writer& header(double) noexcept;

		writer& header(std::string const&) noexcept;
		writer& header(std::string&&) noexcept;
		writer& header(const char*) noexcept;

		writer& header(io) noexcept;

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

		void end() noexcept;

		template<typename ...Args>
		writer& header(Args&&...) noexcept;
		template<typename ...Args>
		writer& column(Args&&...) noexcept;

		template<typename ...Args>
		writer& row(Args&&...) noexcept;
		template<typename ...Args>
		writer& hrow(Args&&...) noexcept;

		template<typename Value>
		writer& operator<<(Value&&) noexcept;
	protected:
		void streambuffer(std::streambuf* sb) noexcept;
	private:
		std::ostream	os_;
		bool			has_col_ = false;

		template<typename Value>
		writer& column_val_impl(Value&&, const char* tag = "td") noexcept;
		writer& column_str_impl(std::string const&, const char* tag = "td") noexcept;

		template<typename Arg>
		void header_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void header_impl(Arg&& arg, Args&& ...args) noexcept;

		template<typename Arg>
		void column_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void column_impl(Arg&& arg, Args&& ...args) noexcept;

		template<typename Arg>
		void row_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void row_impl(Arg&& arg, Args&& ...args) noexcept;

		template<typename Arg>
		void hrow_impl(Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		void hrow_impl(Arg&& arg, Args&& ...args) noexcept;
};

}//CSV

#include "writer_impl.hpp"

#endif /* AGRO_DAEMON_LIB_HTML_WRITE_HPP__ */
