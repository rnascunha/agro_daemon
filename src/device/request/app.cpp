#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../../app/app.hpp"
#include "../../error.hpp"
#include "../message/device.hpp"
#include "../helper.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void get_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	auto dev = instance.device_list()[host];
	if(!dev)
	{
		std::cerr << "Device [" << host.to_string() << "] not found\n";
		return;
	}

	if(CoAP::Message::is_error(response.mcode)
		&& response.mcode != CoAP::Message::code::not_found) //No app found
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		tt::error("[App] Error geting: %.*s [%.*s]",
				response.payload_len, static_cast<const char*>(response.payload),
				host.to_string().size(), host.to_string().data());
		ws->write(instance.make_report(Agro::Message::report_type::error,
					host, p, "GET App", ws->user().id()));
		return;
	}

	if(response.mcode != CoAP::Message::code::not_found)
	{
		::read_packet_app_list(*dev, response.payload, response.payload_len);
	}
	else
	{
		::read_packet_app_list(*dev, nullptr, 0);
	}

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
					std::make_shared<std::string>(Message::device_apps_to_json(*dev)));
}

static void send_app_response(
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
		tt::error("[App] Error sending: %.*s [%.*s]",
						response.payload_len, static_cast<const char*>(response.payload),
						host.to_string().size(), host.to_string().data());
		ws->write(instance.make_report(Agro::Message::report_type::error,
					host, {static_cast<const char*>(response.payload), response.payload_len},
					"GET App", ws->user().id()));
	}
}

static void exec_app_response(
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
		tt::error("[App] Error executing: %.*s [%.*s]",
						response.payload_len, static_cast<const char*>(response.payload),
						host.to_string().size(), host.to_string().data());
		ws->write(instance.make_report(Agro::Message::report_type::error,
					host, {static_cast<const char*>(response.payload), response.payload_len},
					"Exec App", ws->user().id()));
		return;
	}

	std::int32_t arg = *static_cast<const int*>(request.payload);
	std::string app{static_cast<const char*>(request.payload) + sizeof(std::int32_t), request.payload_len - sizeof(std::int32_t)};
	std::stringstream ss;
	ss << "Execute '" << app << "' [arg=" << arg << "]: ret=" << *static_cast<const int*>(response.payload);

	tt::status("[App] Exec %.*s [arg=%d]: ret=%d",
						request.payload_len - sizeof(std::int32_t),
						static_cast<const char*>(request.payload) + sizeof(std::int32_t),
						arg,
						*static_cast<const int*>(response.payload));

	ws->write(instance.make_report(Agro::Message::report_type::info,
						host, ss.str(),
						"Exec App", ws->user().id()));
}

static void delete_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	auto dev = instance.device_list()[host];
	if(!dev)
	{
		std::cerr << "Device [" << host.to_string() << "] not found\n";
		return;
	}

	if(CoAP::Message::is_error(response.mcode))
	{
		tt::error("[App] Error deleting: %.*s [%.*s]",
						response.payload_len, static_cast<const char*>(response.payload),
						host.to_string().size(), host.to_string().data());
		ws->write(instance.make_report(Agro::Message::report_type::error,
					host, {static_cast<const char*>(response.payload), response.payload_len},
					"Delete App", ws->user().id()));
		return;
	}

	dev->delete_app({static_cast<const char*>(request.payload), request.payload_len});
	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
						std::make_shared<std::string>(Message::device_apps_to_json(*dev)));
}

static std::size_t delete_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t,
		instance&,
		std::error_code& ec) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Deleting app missing 'data' field or wrong type");
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	auto const& data = doc["data"].GetObject();

	if(!data.HasMember("app") || !data["app"].IsString())
	{
		tt::warning("Deleting app missing 'app' field or wrong type");
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	const char* app = data["app"].GetString();
	std::size_t s = std::strlen(app);
	std::memcpy(buf, app, s);

	return s;
}

static std::size_t exec_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t,
		instance&,
		std::error_code& ec) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Exec app missing 'data' field or wrong type");
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	auto const& data = doc["data"].GetObject();

	if((!data.HasMember("app") || !data["app"].IsString())
		|| (!data.HasMember("arg") || !data["arg"].IsInt()))
	{
		tt::warning("Exec app missing field or wrong type");
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	const char* app = data["app"].GetString();
	std::size_t s = std::strlen(app);
	std::int32_t arg = data["arg"].GetInt();

	std::memcpy(buf, &arg, sizeof(arg));
	std::memcpy(static_cast<std::uint8_t*>(buf) + sizeof(arg), app, s);

	return s + sizeof(std::int32_t);
}

static std::size_t send_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t,
		instance& instance,
		std::error_code& ec) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Missing 'data' field or wrong type");
		ec = make_error_code(Error::ill_formed);
		return 0;
	}

	auto const data = doc["data"].GetObject();

	if(!data.HasMember("app") || !data["app"].IsString())
	{
		tt::warning("Missing 'app' field or wrong type");
		ec = make_error_code(Error::missing_field);
		return 0;
	}
	const char* app = data["app"].GetString();
	std::size_t s = std::strlen(app);

	sha256_hash hash;
	if(!calculate_app_hash(instance.app_path(), {app, s}, hash))
	{
		tt::error("App '%s' not found", app);
		ec = make_error_code(Error::app_not_found);
		return 0;
	}
	std::memcpy(buf, hash, SHA256_DIGEST_LENGTH);
	std::memcpy(static_cast<std::uint8_t*>(buf) + 32, app, s);

	return s + 32;
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
	exec_app_payload
};

static request_message const req_delete_app = {
	CoAP::Message::code::cdelete,
	{
		{CoAP::Message::Option::code::uri_path, "app"},
	},
	CoAP::Message::type::confirmable,
	delete_app_payload
};

const request_config get_app = {
		{request_type::get_app, "get_app"},
		&req_get_app,
		get_app_response};
const request_config send_app = {
		{request_type::send_app, "send_app"},
		&req_send_app,
		send_app_response};

const request_config execute_app = {
		{request_type::run_app, "execute_app"},
		&req_execute_app,
		exec_app_response};

const request_config delete_app = {
		{request_type::delete_app, "delete_app"},
		&req_delete_app,
		delete_app_response};

}//Request
}//Device
}//Agro
