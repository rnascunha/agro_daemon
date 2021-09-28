#ifndef AGRO_DAEMON_MESSAGE_REQUESTS_IN_PROGRESS_HPP__
#define AGRO_DAEMON_MESSAGE_REQUESTS_IN_PROGRESS_HPP__

#include "../../device/types.hpp"
#include "coap-te.hpp"
#include "request_list.hpp"
#include "../../user/user.hpp"

namespace Message{

struct request_in_progress
{
	request_in_progress(mesh_addr_t const&,
			CoAP::Message::code,
			requests,
			Agro::User::user_id);

	mesh_addr_t		addr;
	CoAP::Message::code method;
	requests		request;
	Agro::User::user_id id;

	bool is_equal(mesh_addr_t const&, CoAP::Message::code, requests) const noexcept;
};

class Request_in_Pogress_List
{
	public:
		bool has(mesh_addr_t const&,
				CoAP::Message::code,
				requests) const noexcept;

		bool add(mesh_addr_t const&,
				CoAP::Message::code,
				requests, Agro::User::user_id) noexcept;
		bool remove(mesh_addr_t const&,
				CoAP::Message::code,
				requests) noexcept;
	private:
		std::vector<request_in_progress> list_;
};


}//Message

#endif /* AGRO_DAEMON_MESSAGE_REQUESTS_IN_PROGRESS_HPP__ */
