#include "types.hpp"
#include <iostream>
#include "../../websocket.hpp"

namespace Message{

static constexpr const unsigned app_max_name_size = 32;

struct app{
	char		name[app_max_name_size];
	unsigned 	size = 0;
};

static void process_get_app(Device_List& device_list,
		mesh_addr_t const& host,
		const void* data, std::size_t data_len) noexcept
{
	auto const dev = device_list[host];
	if(!dev)
	{
		std::cerr << "Device " << host.to_string() << " not found\n";
		return;
	}

	const std::uint8_t* data8 = static_cast<uint8_t const*>(data);
	for(std::size_t i = 0; i < data_len; i += sizeof(app))
	{
		const app* mapp = reinterpret_cast<const app*>(data8);
		dev->add_app(mapp->name, mapp->size);
		data8 += sizeof(app);
	}

	Websocket<false>::write_all(device_apps_to_json(*dev));
}

static void process_send_app(CoAP::Message::message const& response,
		mesh_addr_t const& host) noexcept
{
	Websocket<false>::write_all(make_info(info::info, host, "Send app initiated"));
}

static void get_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error get app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
		Websocket<false>::write_all(
						make_info(info::warning, host, p.c_str())
				);
		return;
	}
	process_get_app(device_list,
			host,
			response.payload, response.payload_len);
}

static void send_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List&) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error send app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
		Websocket<false>::write_all(
								make_info(info::warning, host, p.c_str())
						);
		return;
	}
	process_send_app(response, host);
}

static void exec_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error exec app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
		Websocket<false>::write_all(
								make_info(info::warning, host, p.c_str())
						);
		return;
	}

	std::stringstream ss;
	ss << "Exec app "
		<< std::string{static_cast<const char*>(request.payload), request.payload_len}
		<< " ["
		<< *static_cast<const int*>(response.payload)
		<< "]";

	std::cout << "Output: " << ss.str() << "\n";
	Websocket<false>::write_all(
							make_info(info::success, host, ss.str().c_str()));
}

static void delete_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error delete app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
		Websocket<false>::write_all(
								make_info(info::warning, host, p.c_str())
						);
		return;
	}
	auto const dev = device_list[host];
	dev->delete_app(std::string{static_cast<const char*>(request.payload), request.payload_len});
	Websocket<false>::write_all(device_apps_to_json(*dev));
}

static std::size_t send_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		std::error_code&)
{
	std::size_t s = 0;
	if(doc.HasMember("payload") && doc["payload"].IsString())
	{
		const char* c = doc["payload"].GetString();
		s = std::strlen(c);
		std::memcpy(buf, c, s);
	}
	return s;
}

static request_message const req_get_app = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "app"},
	}
};

static request_message const req_send_app = {
	CoAP::Message::code::post,
	{
		{CoAP::Message::Option::code::uri_path, "app"},
	},
	CoAP::Message::type::confirmable,
	send_app_payload
};

static request_message const req_execute_app = {
	CoAP::Message::code::put,
	{
		{CoAP::Message::Option::code::uri_path, "app"},
	},
	CoAP::Message::type::confirmable,
	send_app_payload
};

static request_message const req_delete_app = {
	CoAP::Message::code::cdelete,
	{
		{CoAP::Message::Option::code::uri_path, "app"},
	},
	CoAP::Message::type::confirmable,
	send_app_payload
};

extern constexpr const request_config get_app = {
		requests::get_app,
		"get_app",
		&req_get_app,
		get_app_response};
extern constexpr const request_config send_app = {
		requests::send_app,
		"send_app",
		&req_send_app,
		send_app_response};

extern constexpr const request_config execute_app = {
		requests::run_app,
		"execute_app",
		&req_execute_app,
		exec_app_response};

extern constexpr const request_config delete_app = {
		requests::delete_app,
		"delete_app",
		&req_delete_app,
		delete_app_response};

}//Message
