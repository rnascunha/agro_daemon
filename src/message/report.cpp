#include "report.hpp"
#include "make.hpp"
#include "rapidjson/document.h"

namespace Agro{
namespace Message{

constexpr const ::Message::config<report_type> report_type_arr[] = {
	{report_type::error, "error"},
	{report_type::warning, "warning"},
	{report_type::success, "success"},
	{report_type::info, "info"},
};

inline constexpr ::Message::config<report_type> const* get_config(report_type t) noexcept
{
	for(std::size_t i = 0; i < sizeof(report_type_arr) / sizeof(report_type_arr[0]); i++)
	{
		if(t == report_type_arr[i].mtype) return &report_type_arr[i];
	}
	return nullptr;
}

template<typename Allocator>
static void add_report_command(rapidjson::Value& value,
		report_commands type,
		Allocator& alloc) noexcept
{
	auto const* config = get_config(type);
	if(!config) return;

	value.AddMember("command",
			rapidjson::Value(config->name, alloc).Move(),
			alloc);
}

template<typename Allocator>
static void add_report_type(rapidjson::Value& value,
		report_type type,
		Allocator& alloc) noexcept
{
	auto const* config = get_config(type);
	if(!config) return;

	value.AddMember("type",
			rapidjson::Value(config->name, alloc).Move(),
			alloc);
}

template<typename Allocator>
static void set_data_report(rapidjson::Value& data, report const& report, Allocator& alloc) noexcept
{
	data.SetObject();

	add_report_type(data, report.type, alloc);
	data.AddMember("message",
			rapidjson::Value(report.message.data(), report.message.size(), alloc).Move(),
			alloc);
	switch(report.commands)
	{
		case report_commands::device:
			data.AddMember("device",
				rapidjson::Value(report.reference.data(), report.reference.size(), alloc).Move(),
				alloc);
			break;
		default:
			break;
	}

	data.AddMember("time", report.time, alloc);

	if(!report.arg.empty())
	{
		data.AddMember("arg",
				rapidjson::Value(report.arg.data(), report.arg.size(), alloc).Move(),
				alloc);
	}
}

report::report(report_commands command,
			report_type type_,
			std::string const& reference_,
			std::string const& message_,
			std::string const& arg_,
			long time_)
 : commands(command), type(type_), reference(reference_),
   message(message_), arg(arg_), time(time_){}

std::string report_message(report_type type,
		mesh_addr_t const& addr,
		std::string const& message,
		std::string const& arg /* = "" */) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::report);
	add_report_command(doc, report_commands::device, doc.GetAllocator());

	rapidjson::Value data;
	data.SetObject();

	add_report_type(data, type, doc.GetAllocator());
	data.AddMember("message",
			rapidjson::Value(message.data(), message.size(), doc.GetAllocator()).Move(),
			doc.GetAllocator());
	::Message::add_device(data, addr, doc.GetAllocator());

	if(!arg.empty())
	{
		data.AddMember("arg",
				rapidjson::Value(arg.data(), arg.size(), doc.GetAllocator()).Move(),
				doc.GetAllocator());
	}

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

std::string report_message(std::vector<report> const& reports) noexcept
{
	rapidjson::Document doc;
	doc.SetObject();

	::Message::add_type(doc, ::Message::type::report);
	add_report_command(doc, report_commands::list, doc.GetAllocator());


	rapidjson::Value data;
	data.SetArray();

	for(auto const& r : reports)
	{
		rapidjson::Value report;
		set_data_report(report, r, doc.GetAllocator());
		data.PushBack(report, doc.GetAllocator());
	}

	::Message::add_data(doc, data);

	return ::Message::stringify(doc);
}

}//Message
}//Agro
