#ifndef AGRO_DAEMON_MESSAGE_REPORT_HPP__
#define AGRO_DAEMON_MESSAGE_REPORT_HPP__

#include <string>
#include "types.hpp"
#include "../device/types.hpp"

namespace Agro{
namespace Message{

enum class report_commands{
	list = 0,
	device
};

enum class report_type{
	error = 0,
	warning = 1,
	success = 2,
	info = 3
};

struct report{
	report(report_commands,
			report_type,
			std::string const&,
			std::string const&,
			std::string const&,
			long);

	report_commands commands;
	report_type	type;
	std::string reference;
	std::string message;
	std::string arg;
	long		time;
};


constexpr const ::Message::config<report_commands> report_config[] = {
	{report_commands::list, "list"},
	{report_commands::device, "device"},
};

inline constexpr ::Message::config<report_commands> const* get_config(report_commands t) noexcept
{
	for(std::size_t i = 0; i < sizeof(report_config) / sizeof(report_config[0]); i++)
	{
		if(t == report_config[i].mtype) return &report_config[i];
	}
	return nullptr;
}

inline constexpr ::Message::config<report_commands> const* get_report_config(const char* t) noexcept
{
	for(std::size_t i = 0; i < sizeof(report_config) / sizeof(report_config[0]); i++)
	{
		if(std::strcmp(t, report_config[i].name) == 0) return &report_config[i];
	}
	return nullptr;
}

std::string report_message(report_type,
		mesh_addr_t const&,
		std::string const& /* message */,
		std::string const& = ""/* arg */) noexcept;

std::string report_message(std::vector<report> const&) noexcept;

}//Message
}//Agro

#endif /* AGRO_DAEMON_MESSAGE_REPORT_HPP__ */
