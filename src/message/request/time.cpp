#include "types.hpp"
#include <iostream>
#include "../../websocket.hpp"

namespace Message{

static void process_rtc_time(Device_List& device_list,
		mesh_addr_t const& host,
		value_time time) noexcept
{
	auto& dev = device_list.update_rtc_time(host, time);
	Websocket<false>::write_all(device_rtc_time_to_json(dev));
}

static void get_rtc_response(
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
		std::cerr << "Get RTC error[" << response.payload_len << "]: " << p << "\n";
		Websocket<false>::write_all(
				make_info(info::warning, host, p.c_str())
		);
		return;
	}
	process_rtc_time(device_list, host, *static_cast<const value_time*>(response.payload));
}

static void process_fuse(Device_List& device_list,
		mesh_addr_t const& host,
		std::int32_t time) noexcept
{
	auto& dev = device_list.update_fuse(host, time);
	Websocket<false>::write_all(device_fuse_to_json(dev));
}

static void update_rtc_response(
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
		std::cerr << "RTC update error[" << response.payload_len << "]: " << p << "\n";
		Websocket<false>::write_all(
				make_info(info::warning, host, p.c_str())
		);
		return;
	}
	Websocket<false>::write_all(make_info(info::success, host, "RTC update"));
}

static void get_fuse_response(
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
		std::cerr << "Get FUSE error[" << response.payload_len << "]: " << p << "\n";
		Websocket<false>::write_all(
				make_info(info::warning, host, p.c_str())
		);
		return;
	}
	process_fuse(device_list, host, *static_cast<const int32_t*>(response.payload));
}

static void update_fuse_response(
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
		std::cerr << "Update FUSE error[" << response.payload_len << "]: " << p << "\n";
		Websocket<false>::write_all(
				make_info(info::warning, host, p.c_str())
		);
		return;
	}
	Websocket<false>::write_all(make_info(info::success, host, "Fuse update"));
}

static std::size_t rtc_update_payload(rapidjson::Document const&, void* buf, std::size_t size, std::error_code&)
{
	std::uint32_t time = get_time();
	std::memcpy(buf, &time, sizeof(std::uint32_t));

	return sizeof(std::uint32_t);
}

static std::size_t update_fuse_payload(rapidjson::Document const& doc, void* buf, std::size_t size, std::error_code& ec)
{
	if(doc.HasMember("payload") && doc["payload"].IsInt())
	{
		std::int32_t c = doc["payload"].GetInt();
		std::memcpy(buf, &c, sizeof(std::int32_t));
		return sizeof(std::int32_t);
	}
	ec = make_error_code(Error::invalid_value);
	return 0;
}

static CoAP::Message::content_format rtc_update_content = CoAP::Message::content_format::application_octet_stream;
static CoAP::Message::content_format rtc_get_content = CoAP::Message::content_format::application_octet_stream;

static request_message const req_update_rtc = {
	CoAP::Message::code::put,
	{
		{rtc_update_content},
		{CoAP::Message::Option::code::uri_path, "rtc"}
	},
	CoAP::Message::type::confirmable,
	rtc_update_payload
};

static request_message const req_get_rtc = {
	CoAP::Message::code::get,
	{
		{rtc_get_content, true},
		{CoAP::Message::Option::code::uri_path, "rtc"},
	}
};

static CoAP::Message::content_format fuse_get_content = CoAP::Message::content_format::application_octet_stream;
static CoAP::Message::content_format fuse_update_content = CoAP::Message::content_format::application_octet_stream;

static request_message const req_get_fuse = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "fuse"},
		{fuse_get_content, true}
	}
};

static request_message const req_update_fuse = {
	CoAP::Message::code::put,
	{
		{CoAP::Message::Option::code::uri_path, "fuse"},
		{fuse_update_content}
	},
	CoAP::Message::type::confirmable,
	update_fuse_payload
};

extern constexpr const request_config get_rtc = {
	requests::get_rtc,
	"get_rtc",
	&req_get_rtc,
	get_rtc_response
};
extern constexpr const request_config update_rtc = {
	requests::update_rtc,
	"update_rtc",
	&req_update_rtc,
	update_rtc_response
};
extern constexpr const request_config get_fuse = {
	requests::get_fuse,
	"get_fuse",
	&req_get_fuse,
	get_fuse_response
};
extern constexpr const request_config update_fuse = {
	requests::update_fuse,
	"update_fuse",
	&req_update_fuse,
	update_fuse_response
};

}//Message
