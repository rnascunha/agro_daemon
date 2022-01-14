#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../../app/app.hpp"
#include "../../error.hpp"
#include "../message/device.hpp"

namespace Agro{
namespace Device{
namespace Request{

static constexpr const unsigned app_max_name_size = 12;

struct app{
	char		name[app_max_name_size + 1];
	unsigned 	size = 0;
};

static void process_get_app(Agro::Device::Device_List& device_list,
		mesh_addr_t const& host,
		const void* data, std::size_t data_len,
		Agro::websocket_ptr ws) noexcept
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

	ws->write_all(Message::device_apps_to_json(*dev));
}

static void process_send_app(CoAP::Message::message const& response,
		mesh_addr_t const& host,
		Agro::websocket_ptr ws) noexcept
{
//	ws->write_all(::Message::make_info(::Message::info::info, host, "Send app initiated"));
}

static void get_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error get app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::warning, host, p.c_str())
//				);
		return;
	}
	process_get_app(instance.device_list(),
			host,
			response.payload, response.payload_len, ws);
}

static void send_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance&,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error send app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::warning, host, p.c_str())
//						);
		return;
	}
	process_send_app(response, host, ws);
}

static void exec_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance&,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error exec app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::warning, host, p.c_str())
//						);
		return;
	}

	std::int32_t arg = *static_cast<const int*>(request.payload);
	std::string app{static_cast<const char*>(request.payload) + sizeof(std::int32_t), request.payload_len - sizeof(std::int32_t)};
	std::stringstream ss;
	ss << "Exec app "
		<< app
		<< "(arg=" << arg << ") "
		<< " [ret="
		<< *static_cast<const int*>(response.payload)
		<< "]";

//	ws->write_all(::Message::make_info(::Message::info::success, host, ss.str().c_str()));
}

static void delete_app_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Error delete app["
				<< host.to_string()
				<< "]: "
				<< p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::warning, host, p.c_str())
//						);
		return;
	}
	auto const dev = instance.device_list()[host];
	dev->delete_app({static_cast<const char*>(request.payload), request.payload_len});
	ws->write_all(Agro::Device::Message::device_apps_to_json(*dev));
}

static std::size_t name_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		instance&,
		std::error_code&) noexcept
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

static std::size_t exec_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		instance&,
		std::error_code& ec) noexcept
{
	std::size_t s = 0;
	if(doc.HasMember("payload") && doc["payload"].IsObject())
	{
		auto const& payload = doc["payload"].GetObject();
		std::int32_t arg = 0;
		const char* c;
		if(payload.HasMember("arg") && payload["arg"].IsInt())
		{
			arg = payload["arg"].GetInt();
		}
		if(payload.HasMember("app") && payload["app"].IsString())
		{
			c = payload["app"].GetString();
			s = std::strlen(c);
		}
		else
		{
			ec = make_error_code(Error::missing_field);
			return 0;
		}
		std::memcpy(buf, &arg, sizeof(arg));
		std::memcpy(static_cast<std::uint8_t*>(buf) + sizeof(arg), c, s);
	}
	else
	{
		ec = make_error_code(Error::invalid_value);
		return 0;
	}
	return s + sizeof(std::int32_t);
}

static std::size_t send_app_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t size,
		instance& instance,
		std::error_code& ec) noexcept
{
	std::size_t s = 0;
	if(doc.HasMember("payload") && doc["payload"].IsString())
	{
		const char* c = doc["payload"].GetString();
		s = std::strlen(c);
		sha256_hash hash;
		if(!calculate_app_hash(instance.app_path(), {c, s}, hash))
		{
			ec = make_error_code(Error::app_not_found);
			return 0;
		}
		std::memcpy(buf, hash, SHA256_DIGEST_LENGTH);
		std::memcpy(static_cast<std::uint8_t*>(buf) + 32, c, s);
	}
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
	name_app_payload
};

extern constexpr const request_config get_app = {
		type::get_app,
		"get_app",
		&req_get_app,
		get_app_response};
extern constexpr const request_config send_app = {
		type::send_app,
		"send_app",
		&req_send_app,
		send_app_response};

extern constexpr const request_config execute_app = {
		type::run_app,
		"execute_app",
		&req_execute_app,
		exec_app_response};

extern constexpr const request_config delete_app = {
		type::delete_app,
		"delete_app",
		&req_delete_app,
		delete_app_response};

}//Request
}//Device
}//Agro
