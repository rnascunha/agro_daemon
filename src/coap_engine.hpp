#ifndef AGRO_DAEMON_COAP_ENGINE_HPP__
#define AGRO_DAEMON_COAP_ENGINE_HPP__

#include "coap-te.hpp"			//Convenient header
#include "coap-te/transmission/transaction_list_vector.hpp"

#define BUFFER_LEN		512

using endpoint = CoAP::Port::POSIX::endpoint_ipv4;

/**
 * UDP socket type that will forward packtes from networks
 */
using udp_conn = CoAP::Port::POSIX::udp<endpoint>;


/**
 * Engine definition. Check 'raw_engine' example for a full
 * description os the options.
 *
 * Here we are using server profile, and defining a resource callback
 * function. Server profile allow to add resources. We are also disabling
 * the default callback.
 */
using engine = CoAP::Transmission::engine<
		udp_conn,
		CoAP::Message::message_id,
		CoAP::Transmission::transaction_list_vector<
					CoAP::Transmission::transaction<
						BUFFER_LEN,
						CoAP::Transmission::transaction_cb,
						endpoint>
		>,
		CoAP::disable,		//default callback disabled
		CoAP::Resource::resource<
			CoAP::Resource::callback<endpoint>,
			true
		>
	>;

#endif /* AGRO_DAEMON_COAP_ENGINE_HPP__ */
