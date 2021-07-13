#include "types.hpp"
#include <iostream>
#include "../../websocket.hpp"

namespace Message{

static void process_get_ota(Device_List& device_list,
		mesh_addr_t const& host,
		std::string&& version) noexcept
{
	auto const dev = device_list[host];
	if(!dev)
	{
		std::cerr << "Device " << host.to_string() << " not found\n";
		return;
	}
	Websocket<false>::write_all(device_ota_to_json(*dev, version));
}

static void process_update_ota(CoAP::Message::message const& response,
		mesh_addr_t const& host) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Update OTA error[" << response.payload_len << "]: " << p << "\n";
		Websocket<false>::write_all(
				make_info(info::warning, host, p.c_str())
		);
		return;
	}
	Websocket<false>::write_all(make_info(info::info, host, "OTA update initiated"));
}

static void get_ota_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List& device_list) noexcept
{
	process_get_ota(device_list,
			host,
			std::string{static_cast<const char*>(response.payload), response.payload_len});
}

static void update_ota_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		requests,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Device_List&) noexcept
{
	process_update_ota(response, host);
}

static std::size_t update_ota_payload(
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

static request_message const req_get_ota = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "ota"},
	}
};

static request_message const req_update_ota = {
	CoAP::Message::code::post,
	{
		{CoAP::Message::Option::code::uri_path, "ota"},
	},
	CoAP::Message::type::confirmable,
	update_ota_payload
};

extern constexpr const request_config get_ota = {
		requests::get_ota,
		"get_ota_version",
		&req_get_ota,
		get_ota_response};
extern constexpr const request_config update_ota = {
		requests::update_ota,
		"update_ota",
		&req_update_ota,
		update_ota_response};

}//Message
