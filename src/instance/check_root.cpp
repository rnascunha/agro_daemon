#include "agro.hpp"
#include "tt/tt.hpp"
#include "coap-te-debug.hpp"
#include "../resources/process.hpp"
#include "../user/policy.hpp"

static void request_route_cb(void const* trans,
		CoAP::Message::message const* response,
		void* instance) noexcept;

static constexpr const CoAP::Transmission::configure config = {
		/* .ack_timeout_seconds 			= */ 1.5,	//ACK_TIMEOUT
		/* .ack_random_factor 				= */ 1.5,	//ACK_RANDOM_FACTOR
		/* .max_restransmission 			= */ 1,	//MAX_RETRANSMIT
};

namespace Agro{

void instance::initiate_check_roots() noexcept
{
	check_root_timer_.expires_after(std::chrono::seconds(5));
	check_root_timer_.async_wait([this](const boost::system::error_code&){
//		tt::debug("Checking roots!");
		check_network_roots();
		initiate_check_roots();
	});
}

void instance::check_root(Device::Tree::tree_endpoint& ep) noexcept
{
	engine::request req{ep.ep};
	req.header(CoAP::Message::type::confirmable, CoAP::Message::code::get);

	char str[mesh_addr_t::str_size];
	CoAP::Message::Option::node host{CoAP::Message::Option::code::uri_host,
									ep.addr.to_string(str, mesh_addr_t::str_size)},
								path_net{CoAP::Message::Option::code::uri_path,
										"net"},
								path_route{CoAP::Message::Option::code::uri_path,
										"route"};

	req.add_option(host)
		.add_option(path_net)
		.add_option(path_route);

	req.callback(request_route_cb, this);

	CoAP::Error ec;
	coap_engine_.send(req, config, ec);
	if(ec)
	{
		char addr_str[18], ep_str[18];
		tt::error("Error sending 'check route'! [%s/%s][%d/%s]",
				ep.addr.to_string(addr_str, 18), ep.ep.address(ep_str, 18),
				ec.value(), ec.message());
		return;
	}
	ep.checking = true;
}

void instance::check_network_roots() noexcept
{
	auto& tree = tree_.get_endpoint();
	for(auto& [addr, ep] : tree)
	{
		if(!ep.checking)
		{
			check_root(ep);
		}
	}
}

}//Agro


static void request_route_cb(void const* trans,
		CoAP::Message::message const* response,
		void* instance_ptr) noexcept
{
	auto const* t = static_cast<engine::transaction_t const*>(trans);
	Agro::instance* instance = static_cast<Agro::instance*>(instance_ptr);

	CoAP::Message::Option::option op;
	if(!CoAP::Message::Option::get_option(t->request(), op, CoAP::Message::Option::code::uri_host))
	{
		/**
		 * This is a impossible possibility! Something is wrong and must be
		 * checked
		 */
		tt::error("['check_root'] Option uri-host not found!");
//		return;
	}

	std::error_code ec;
	::mesh_addr_t host{static_cast<const char*>(op.value), op.length, ec};

	bool change = false;
	if(response)
	{
		tt::debug("Response 'check root' received!");
//		CoAP::Debug::print_message_string(*response);
		auto* dev = instance->device_list()[host];
		if(!dev)
		{
			tt::error("['check_root'] Device not found!");
			goto end;
		}

		std::error_code ec;
		::Resource::process_route(*dev,
					instance->share(),
					*instance,
					t->endpoint(),
					response->payload, response->payload_len,
					ec, false);
	}
	else
	{
		/**
		 * Function timeout or transaction was cancelled
		 */
		tt::debug("Response 'check root' NOT received");

		tt::debug("Removing node from tree...");
		change = instance->remove_node_from_tree(host);
	}
end:
	instance->tree().print_all();
	instance->tree().uncheck_endpoint(host);
	std::cout << "Tree" << (change ? "" : " NOT") << " changed\n";
}

