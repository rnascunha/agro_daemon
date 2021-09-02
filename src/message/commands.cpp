#include "process.hpp"
#include <iostream>
#include "types.hpp"
#include "../websocket/websocket.hpp"
#include "device.hpp"

namespace Message{

void process_commands(rapidjson::Document const& d, Device_List& list) noexcept
{
	std::cout << "Processing commands\n";
	if(!(d.HasMember("device") && d["device"].IsString()))
	{
		std::cerr << "Device not set\n";
		return;
	}
	const char* mac = d["device"].GetString();

	std::error_code ec;
	mesh_addr_t addr(mac, std::strlen(mac), ec);
	if(ec)
	{
		std::cerr << "Error: " << ec.value() << " " << ec.message() << "\n";
		return;
	}

	if(!(d.HasMember("comm") && d["comm"].IsString()))
	{
		std::cerr << "Command not found...00\n";
		return;
	}
	auto const* command = Message::get_commands_config(d["comm"].GetString());
	if(!command)
	{
		std::cerr << "Command not found...\n";
		return;
	}

	switch(command->mtype)
	{
		case Message::commands::update_name:
		{
			if(!(d.HasMember("data") && d["data"].IsString()))
			{
				std::cerr << "Command update_name incomplete...\n";
				return;
			}

			std::string name{d["data"].GetString()};
			auto& dev = list.update_name(addr, name);
			Websocket<false>::write_all(Message::device_to_json(dev));
			std::cout << "message updated[" << mac << "]: " << name << "\n";
		}
			break;
		default:
			std::cerr << "Command not found...\n";
			break;
	}
}

}//Message
