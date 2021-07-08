#include "process.hpp"
#include <iostream>
#include "types.hpp"
#include "coap-te-debug.hpp"
#include "make.hpp"
#include "../websocket.hpp"
#include "../resources/process.hpp"
#include "info.hpp"

namespace Message{

static CoAP::Message::code string_to_method(const char* method_str)
{
	std::string s{method_str};
	if(s == "get") return CoAP::Message::code::get;
	if(s == "post") return CoAP::Message::code::post;
	if(s == "put") return CoAP::Message::code::put;
	if(s == "del") return CoAP::Message::code::cdelete;

	return CoAP::Message::code::bad_request;
}

static void process_ac_load(Device_List& device_list,
		mesh_addr_t const& host,
		unsigned index, bool value) noexcept
{
	auto& dev = device_list.update_ac_load(host, index, value);
	Websocket<false>::write_all(device_gpios_to_json(dev));
}

static void process_rtc_time(Device_List& device_list,
		mesh_addr_t const& host,
		value_time time) noexcept
{
	auto& dev = device_list.update_rtc_time(host, time);
	Websocket<false>::write_all(device_rtc_time_to_json(dev));
}

static void process_uptime(Device_List& device_list,
		mesh_addr_t const& host,
		int64_t uptime) noexcept
{
	auto& dev = device_list.update_uptime(host, uptime);
	Websocket<false>::write_all(device_uptime_to_json(dev));
}

static void process_ota(Device_List& device_list,
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

static void request_response(
		engine::endpoint const& ep,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t status,
		requests req,
		Device_List& device_list) noexcept
{
	request_config const* config = get_requests_config(req);
	if(!config)
	{
		std::cerr << "Response request not defiend\n";
		return;
	}

	CoAP::Message::Option::option op;
	CoAP::Message::Option::get_option(response, op, CoAP::Message::Option::code::uri_host);

	std::error_code ec;
	::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};
	if(ec) return;

	switch(req)
	{
		case requests::custom: {
			rapidjson::Document doc;
			Message::make_response(doc, request, response, ep, status);
			Websocket<false>::write_all(Message::stringify(doc));
		}
			break;
		case requests::ac_load1_on:
		case requests::ac_load1_off: {
			process_ac_load(device_list, host, 0, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
		}
		break;
		case requests::ac_load2_on:
		case requests::ac_load2_off:
			process_ac_load(device_list, host, 1, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
			break;
		case requests::ac_load3_on:
		case requests::ac_load3_off:
			process_ac_load(device_list, host, 2, static_cast<bool>(*static_cast<const uint8_t*>(response.payload) - '0'));
			break;
		case requests::sensor: {
			std::error_code ec;
			Resource::process_sensor_data(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
			break;
		case requests::board: {
			std::error_code ec;
			Resource::process_board(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::config: {
			std::error_code ec;
			Resource::process_board(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::full_config: {
			std::error_code ec;
			Resource::process_full_config(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::route: {
			std::error_code ec;
			Resource::process_route(device_list,
					ep, op,
					response.payload, response.payload_len,
					ec);
		}
		break;
		case requests::uptime:
		{
			std::string time{static_cast<const char*>(response.payload), response.payload_len};
			process_uptime(device_list, host, std::strtoll(time.c_str(), nullptr, 10));
		}
			break;
		case requests::get_rtc:
			process_rtc_time(device_list, host, *static_cast<const value_time*>(response.payload));
			break;
		case requests::update_rtc:
			Websocket<false>::write_all(make_info(info::success, host, "RTC update"));
			break;
		case requests::get_ota:
			process_ota(device_list, host,
					std::string{static_cast<const char*>(response.payload), response.payload_len});
			break;
		case requests::update_ota:
			Websocket<false>::write_all(make_info(info::info, host, "OTA update initiated"));
			break;
		default:
			break;
	}
}

/**
 * Request callback (signature defined at transaction)
 */
static void request_cb(void const* trans,
		CoAP::Message::message const* response,
		void* request,
		Device_List& dev_list) noexcept
{
	std::cout << "Request callback called...\n";

	auto const* t = static_cast<engine::transaction_t const*>(trans);
	std::cout << "Status: " << static_cast<int>(t->status()) << "\n";
	if(response)
	{
		std::cout << "Response received!\n\n";
		CoAP::Debug::print_message_string(*response);

		auto req = reinterpret_cast<std::uintptr_t>(request);
		engine::endpoint const ep = t->endpoint();
		request_response(ep,
				t->request(),
				*response,
				t->status(),
				static_cast<Message::requests>(req),
				dev_list);
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		std::cout << "Response NOT received\n";

		rapidjson::Document doc;
		Message::make_response(doc, t->request(), t->endpoint(), t->status());

		std::string resp_str{Message::stringify(doc)};
		std::cout << resp_str << "\n";

		Websocket<false>::write_all(resp_str);
	}
}

static void process_custom_request(rapidjson::Document const& d,
							Message::request_message& msg,
							std::error_code& ec) noexcept
{
	if(!d.HasMember("method") || !d["method"].IsString())
	{
		ec = make_error_code(Error::missing_field);
		return;
	}

	msg.method = string_to_method(d["method"].GetString());
	if(msg.method == CoAP::Message::code::bad_request)
	{
		ec = make_error_code(Error::invalid_value);
		return;
	}

	if(d.HasMember("resource") && d["resource"].IsArray())
	{
		rapidjson::Value const& resource = d["resource"];
		for (auto& v : resource.GetArray())
		{
			msg.options.emplace_back(CoAP::Message::Option::code::uri_path, v.GetString());
		}
	}

	if(d.HasMember("query") && d["query"].IsArray())
	{
		rapidjson::Value const& query = d["query"];
		for (auto& v: query.GetArray())
		{
			msg.options.emplace_back(CoAP::Message::Option::code::uri_query, v.GetString());
		}
	}

	if(d.HasMember("payload") && !d["payload"].IsNull())
	{
		msg.payload = [](rapidjson::Document const& d, void* buf, std::size_t size){
			const char* payload = d["payload"].GetString();
			std::size_t s = std::strlen(payload);

			std::memcpy(buf, payload, s);
			return s;
		};
	}
}

static void send_request(
		const char* mac,
		engine::endpoint const& ep,
		Message::requests request_type,
		Message::request_message const& msg,
		engine& coap_engine,
		Device_List& dev_list,
		rapidjson::Document const& doc) noexcept
{
	engine::request req{ep};
	req.header(msg.mtype, msg.method);

	CoAP::Message::Option::node host{CoAP::Message::Option::code::uri_host, mac};

	std::vector<CoAP::Message::Option::node> v;
	for(auto const& op : msg.options)
		v.emplace_back(op);

	req.add_option(host);
	for(auto& op : v)
	{
		req.add_option(op);
	}

	std::uint8_t buffer[engine::packet_size];
	std::size_t size = 0;
	if(msg.payload)
	{
		size = msg.payload(doc, buffer, engine::packet_size);
		if(size > 0)
		{
			req.payload(buffer, size);
		}
	}

	req.callback(std::bind(request_cb,
				std::placeholders::_1,
				std::placeholders::_2,
				std::placeholders::_3,
				std::ref(dev_list)),
			reinterpret_cast<int*>(request_type));

	CoAP::Error ec;
	coap_engine.send(req, ec);
	if(ec)
	{
		std::cerr << "Send Request error[" << ec.value() << "/" << ec.message() << "]\n";
	}
}

void process_request(rapidjson::Document const& doc,
			Device_List& device_list,
			engine& coap_engine) noexcept
{
	if(!doc.HasMember("request") || !doc["request"].IsString())
	{
		std::cerr << "Not valid request\n";
		return;
	}

	Message::request_config const* config = Message::get_requests_config(doc["request"].GetString());
	std::cout << "Request search: " << doc["request"].GetString() << "\n";
	if(!config)
	{
		std::cerr << "Request config not found\n";
		return;
	}

	if(doc.HasMember("device") && !doc["device"].IsString())
	{
		std::cerr << "Request device not found\n";
		return;
	}
	Device const* dev = device_list[doc["device"].GetString()];
	if(!dev)
	{
		std::cerr << "Device not found\n";
		return;
	}

	Message::request_message const* req = config->message;
	Message::request_message custom_req;
	if(config->mtype == Message::requests::custom)
	{
		std::error_code ec;
		process_custom_request(doc, custom_req, ec);
		if(ec)
		{
			std::cerr << "Error processing custom request\n";
			return;
		}
		req = &custom_req;
	}
	std::cout << "Req: " << static_cast<int>(config->mtype) << " " << config->name << "\n";

	send_request(doc["device"].GetString(),
			dev->get_endpoint(),
			config->mtype,
			*req,
			coap_engine,
			device_list,
			doc);
}

}//Message
