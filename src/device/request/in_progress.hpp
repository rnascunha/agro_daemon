#ifndef AGRO_DAEMON_DEVICE_REQUESTS_IN_PROGRESS_HPP__
#define AGRO_DAEMON_DEVICE_REQUESTS_IN_PROGRESS_HPP__

#include "../../device/types.hpp"
#include "coap-te.hpp"
#include "request_list.hpp"
#include "../../user/user.hpp"

namespace Agro{
namespace Device{
namespace Request{

struct request_in_progress
{
	request_in_progress(mesh_addr_t const&,
			CoAP::Message::code,
			request_type,
			Agro::User::user_id);

	mesh_addr_t		addr;
	CoAP::Message::code method;
	request_type			request;
	Agro::User::user_id id;

	bool is_equal(mesh_addr_t const&, CoAP::Message::code, request_type) const noexcept;
};

class Request_in_Pogress_List
{
	public:
		bool has(mesh_addr_t const&,
				CoAP::Message::code,
				request_type) const noexcept;

		bool add(mesh_addr_t const&,
				CoAP::Message::code,
				request_type, Agro::User::user_id) noexcept;
		bool remove(mesh_addr_t const&,
				CoAP::Message::code,
				request_type) noexcept;
	private:
		std::vector<request_in_progress> list_;
};

}//Request
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_REQUESTS_IN_PROGRESS_HPP__ */
