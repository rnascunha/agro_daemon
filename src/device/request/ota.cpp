#include "types.hpp"
#include "../../message/report.hpp"
#include "../message/device.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_get_ota(instance& instance,
		mesh_addr_t const& host,
		std::string&& version,
		Agro::websocket_ptr ws) noexcept
{
	auto const dev = instance.device_list()[host];
	if(!dev)
	{
		tt::error("OTA GET response error! Device '%s' not found", host.to_string().c_str());
		ws->write(instance.make_report(Agro::Message::report_type::error,
			host, "Device not found", "GET OTA", ws->user().id()));
		return;
	}
	ws->write_all_policy(Authorization::rule::view_device,
			std::make_shared<std::string>(Message::device_ota_to_json(*dev, version)));
}

static void process_update_ota(CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		mesh_addr_t const& host,
		instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("Device '%s' OTA update error [%.*s]",
				host.to_string().c_str(),
				request.payload_len,
				static_cast<const char*>(request.payload));
		ws->write(instance.make_report(Agro::Message::report_type::error,
				host,"Update OTA error",
				std::string{static_cast<const char*>(request.payload), request.payload_len},
				ws->user().id()));
		return;
	}
	tt::status("Device '%s' OTA update initiated [%.*s]",
			host.to_string().c_str(),
			request.payload_len,
			static_cast<const char*>(request.payload));
	ws->write(instance.make_report(Agro::Message::report_type::success,
			host,"Update OTA initiated",
			std::string{static_cast<const char*>(request.payload), request.payload_len},
			ws->user().id()));
}

static void get_ota_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	process_get_ota(instance,
			host,
			{static_cast<const char*>(response.payload), response.payload_len},
			ws);
}

static void update_ota_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	process_update_ota(request, response, host, instance, ws);
}

static std::size_t update_ota_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		instance& instance,
		std::error_code& ec)
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	rapidjson::Value const& data = doc["data"].GetObject();
	if(!data.HasMember("image") || !data["image"].IsString())
	{
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	std::string image_file = data["image"].GetString();
	if(!instance.image_path().has(image_file))
	{
		ec = make_error_code(Error::image_not_found);
		return 0;
	}

	std::memcpy(buf, image_file.data(), image_file.size());
	return image_file.size();
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

const request_config get_ota = {
		{request_type::get_ota, "get_ota_version"},
		&req_get_ota,
		get_ota_response};
const request_config update_ota = {
		{request_type::update_ota, "update_ota"},
		&req_update_ota,
		update_ota_response};

}//Request
}//Device
}//Agro
