#include <iostream>

#include "../../message/types.hpp"
#include "types.hpp"
#include "../../message/report.hpp"

#include "coap-te-debug.hpp"
#include "../../message/make.hpp"
#include "../../websocket/types.hpp"
#include "../../resources/process.hpp"
#include "../../helper/coap_helper.hpp"

namespace Agro{
namespace Device{
namespace Request{

static CoAP::Message::code string_to_method(const char* method_str)
{
	std::string s{method_str};
	if(s == "get") return CoAP::Message::code::get;
	if(s == "post") return CoAP::Message::code::post;
	if(s == "put") return CoAP::Message::code::put;
	if(s == "del") return CoAP::Message::code::cdelete;

	return CoAP::Message::code::bad_request;
}

/**
 * Request callback (signature defined at transaction)
 */
static void request_cb(void const* trans,
		CoAP::Message::message const* response,
		void* request,
		Agro::websocket_ptr ws,
		Agro::instance& instance) noexcept
{
	auto const* t = static_cast<engine::transaction_t const*>(trans);
	auto req = static_cast<type>(reinterpret_cast<std::uintptr_t>(request));

	CoAP::Message::Option::option op;
	if(!CoAP::Message::Option::get_option(t->request(), op, CoAP::Message::Option::code::uri_host))
	{
		/**
		 * This is a impossible possibility! Something is wrong and must be
		 * checked
		 */
		tt::error("Option uri-host not found!");
//		return;
	}
	std::error_code ec;
	::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};

	std::string path = make_coap_path(t->request());

	if(response)
	{
		std::cout << "Response received!\n\n";
		CoAP::Debug::print_message_string(*response);
		engine::endpoint const ep = t->endpoint();
		request_config const* config = get_requests_config(req);
		if(!config)
		{
			goto end;
		}

		ws->write(instance.make_report(Agro::Message::report_type::success,
						host, "Request succeded", path, ws->user().id()));

		if(config->response)
		{
			config->response(ep,
					host,
					req,
					t->request(),
					*response,
					t->status(),
					instance,
					ws);
		}
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		std::cout << "Response NOT received\n";

		ws->write(instance.make_report(Agro::Message::report_type::error,
								host, "Request failed", path, ws->user().id()));

		std::cout << "Removing node from tree...\n";
		instance.remove_node_from_tree(host);
	}
end:
	instance.remove_request_in_progress(host, t->request().mcode, req);
}

static void process_custom_request(rapidjson::Document const& d,
							request_message& msg,
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
		msg.payload = [](rapidjson::Document const& d, void* buf, std::size_t size, std::error_code&){
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
		type request_type,
		request_message const& msg,
		Agro::websocket_ptr ws,
		Agro::instance& instance,
		rapidjson::Document const& doc,
		CoAP::Error& ec) noexcept
{
	engine::request req{ep};
	req.header(msg.mtype, msg.method);

	CoAP::Message::Option::node host{CoAP::Message::Option::code::uri_host, mac};

	std::vector<CoAP::Message::Option::node> v;
	for(auto const& op : msg.options)
	{
		v.emplace_back(op);
	}

	req.add_option(host);
	for(auto& op : v)
	{
		req.add_option(op);
	}

	std::uint8_t buffer[engine::packet_size];
	std::size_t size = 0;
	if(msg.payload)
	{
		std::error_code ec;
		size = msg.payload(doc, buffer, engine::packet_size, ec);
		if(ec) return;
		if(size > 0)
		{
			req.payload(buffer, size);
		}
	}

	req.callback(std::bind(request_cb,
					std::placeholders::_1,
					std::placeholders::_2,
					std::placeholders::_3,
					ws,
					std::ref(instance)),
			reinterpret_cast<int*>(request_type));

	instance.coap_engine().send(req, ec);
}

static bool is_user_allowed(CoAP::Message::code code, Agro::User::Logged& user) noexcept
{
	bool ret = true;
	switch(code)
	{
		case CoAP::Message::code::get:
			if(!Agro::Authorization::can(user, Agro::Authorization::rule::get_resource))
			{
				tt::warning("User '%d' not allowed to send GET requests!", user.id());
				ret = false;
			}
			break;
		case CoAP::Message::code::put:
			if(!Agro::Authorization::can(user, Agro::Authorization::rule::put_resource))
			{
				tt::warning("User '%d' not allowed to send PUT requests!", user.id());
				ret = false;
			}
			break;
		case CoAP::Message::code::post:
			if(!Agro::Authorization::can(user, Agro::Authorization::rule::post_resource))
			{
				tt::warning("User '%d' not allowed to send POST requests!", user.id());
				ret = false;
			}
			break;
		case CoAP::Message::code::cdelete:
			if(!Agro::Authorization::can(user, Agro::Authorization::rule::delete_resource))
			{
				tt::warning("User '%d' not allowed to send DELETE requests!", user.id());
				ret = false;
			}
			break;
		default:
			tt::warning("Wrong request method!");
			ret = false;
	}
	return ret;
}

void process_request(rapidjson::Document const& doc,
				Agro::websocket_ptr ws,
				Agro::instance& instance,
				Agro::User::Logged& user) noexcept
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		tt::warning("Request missing field 'data'.");
		return;
	}

	rapidjson::Value const& data = doc["data"].GetObject();

	if(!data.HasMember("request") || !data["request"].IsString())
	{
		tt::warning("Request missing field 'request'.");
		return;
	}

	request_config const* config = get_requests_config(data["request"].GetString());
	if(!config)
	{
		tt::warning("Invalid request [%s].", data["request"].GetString());
		return;
	}

	if(!data.HasMember("device") || !data["device"].IsString())
	{
		tt::warning("Request missing field 'device'.");
		return;
	}

	Agro::Device::Device const* dev = instance.device_list()[data["device"].GetString()];
	if(!dev)
	{
		tt::warning("Request device not found [%s]", data["device"].GetString());
		return;
	}

	request_message const* req = config->message;
	request_message custom_req;
	if(config->mtype == type::custom)
	{
		std::error_code ec;
		process_custom_request(doc, custom_req, ec);
		if(ec)
		{
			tt::error("Error processing custom request [%d/%s]", ec.value(), ec.message().c_str());
			return;
		}
		req = &custom_req;
	}

	//Checking if user is allowed
	if(!is_user_allowed(req->method, user)) return;

	/**
	 * Checking if already have a request to the same device
	 */
	if(config->mtype != type::custom)
	{
		if(!instance.add_request_in_progress(dev->mac(), req->method, config->mtype, user.id()))
		{
			ws->write(instance.make_report(Agro::Message::report_type::warning,
							dev->mac(), "Request in progress", make_coap_path(req->options), user.id()));
			return;
		}
	}

	CoAP::Error ecc;
	send_request(data["device"].GetString(),
			dev->get_endpoint(),
			config->mtype,
			*req,
			ws,
			instance,
			doc,
			ecc);

	/**
	 * If error sending...
	 */
	if(ecc)
	{
		tt::error("CoAP send error! [%d/%m]", ecc.value(), ecc.message());
		if(config->mtype != type::custom)
		{
			/**
			 * Removing request from in progress...
			 */
			instance.remove_request_in_progress(dev->mac(), req->method, config->mtype);
		}
		switch(static_cast<CoAP::errc>(ecc.value()))
		{
			case CoAP::errc::no_free_slots:
				ws->write(instance.make_report(Agro::Message::report_type::warning,
						dev->mac(), "Server is busy! Wait a moment to make a request", make_coap_path(req->options), user.id()));
				break;
			default:
				std::stringstream msg{"Server error! ["};
				msg << ecc.value() << " / "  << ecc.message() << "]";
				ws->write(instance.make_report(Agro::Message::report_type::error,
						dev->mac(), msg.str(), make_coap_path(req->options), user.id()));
				break;
		}
	}
}

}//Request
}//Device
}//Agro

