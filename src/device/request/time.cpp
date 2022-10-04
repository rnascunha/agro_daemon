#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../message/device.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_rtc_time(Agro::Device::Device_List& device_list,
		mesh_addr_t const& host,
		value_time time,
		Agro::websocket_ptr ws) noexcept
{
	auto* dev = device_list[host];
	dev->update_rtc_time(time);
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Message::device_rtc_time_to_json(*dev)));
}

static void get_rtc_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("[%s] Get RTC error [%.*s]",
				host.to_string().c_str(),
				response.payload_len, static_cast<const char*>(response.payload));
		ws->write(instance.make_report(Agro::Message::report_type::error,
				host,"Get RTC error",
				std::string{static_cast<const char*>(response.payload), response.payload_len},
				ws->user().id()));

		return;
	}
	process_rtc_time(instance.device_list(), host, *static_cast<const value_time*>(response.payload), ws);
}

static void update_rtc_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("[%s] Update RTC error [%.*s]",
				host.to_string().c_str(),
				response.payload_len, static_cast<const char*>(response.payload));
		ws->write(instance.make_report(Agro::Message::report_type::error,
				host,"Update RTC error",
				std::string{static_cast<const char*>(response.payload), response.payload_len},
				ws->user().id()));

		return;
	}
	process_rtc_time(instance.device_list(), host, *static_cast<const value_time*>(request.payload), ws);
}

static void process_fuse(Agro::Device::Device_List& device_list,
		mesh_addr_t const& host,
		std::int32_t time,
		Agro::websocket_ptr ws) noexcept
{
	auto* dev = device_list[host];
	dev->fuse(time);
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Message::device_fuse_to_json(*dev)));
}

static void get_fuse_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("[%s] Get fuse error [%.*s]",
				host.to_string().c_str(),
				response.payload_len, static_cast<const char*>(response.payload));
		ws->write(instance.make_report(Agro::Message::report_type::error,
				host,"Get fuse error",
				std::string{static_cast<const char*>(response.payload), response.payload_len},
				ws->user().id()));
		return;
	}
	process_fuse(instance.device_list(), host, *static_cast<const int32_t*>(response.payload), ws);
}

static void update_fuse_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("[%s] Update fuse error [%.*s]",
				host.to_string().c_str(),
				response.payload_len, static_cast<const char*>(response.payload));
		ws->write(instance.make_report(Agro::Message::report_type::error,
				host,"Update fuse error",
				std::string{static_cast<const char*>(response.payload), response.payload_len},
				ws->user().id()));
		return;
	}
	process_fuse(instance.device_list(), host, *static_cast<const int32_t*>(request.payload), ws);
}

static std::size_t rtc_update_payload(rapidjson::Document const&,
		void* buf,
		std::size_t size,
		instance&,
		std::error_code&)
{
	std::uint32_t time = get_time();
	std::memcpy(buf, &time, sizeof(std::uint32_t));

	return sizeof(std::uint32_t);
}

static std::size_t update_fuse_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		instance&,
		std::error_code& ec)
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		ec = make_error_code(Error::missing_field);
		return 0;
	}
	rapidjson::Value const& data = doc["data"].GetObject();

	if(data.HasMember("payload") && data["payload"].IsInt())
	{
		std::int32_t c = data["payload"].GetInt();
		std::memcpy(buf, &c, sizeof(std::int32_t));
		return sizeof(std::int32_t);
	}
	ec = make_error_code(Error::invalid_value);
	return 0;
}

static constexpr const CoAP::Message::content_format rtc_update_content{CoAP::Message::content_format::application_octet_stream};
static constexpr const CoAP::Message::accept rtc_get_content{CoAP::Message::accept::application_octet_stream};

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
		{rtc_get_content},
		{CoAP::Message::Option::code::uri_path, "rtc"},
	}
};

static constexpr const CoAP::Message::accept fuse_get_content{CoAP::Message::accept::application_octet_stream};
static constexpr const CoAP::Message::content_format fuse_update_content{CoAP::Message::content_format::application_octet_stream};

static request_message const req_get_fuse = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "fuse"},
		{fuse_get_content}
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

const request_config get_rtc = {
	{request_type::get_rtc, "get_rtc"},
	&req_get_rtc,
	get_rtc_response
};
const request_config update_rtc = {
	{request_type::update_rtc, "update_rtc"},
	&req_update_rtc,
	update_rtc_response
};
const request_config get_fuse = {
	{request_type::get_fuse, "get_fuse"},
	&req_get_fuse,
	get_fuse_response
};
const request_config update_fuse = {
	{request_type::update_fuse, "update_fuse"},
	&req_update_fuse,
	update_fuse_response
};

}//Request
}//Device
}//Agro
