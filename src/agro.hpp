#ifndef AGRO_DAEMON_AGRO_HPP__
#define AGRO_DAEMON_AGRO_HPP__

#include "db/db.hpp"
#include "coap_engine.hpp"
#include "device/list.hpp"

namespace Agro{

struct instance{
	DB&				db;
	engine&			coap_engine;
	Device_List&	device_list;
};

}

#endif /* AGRO_DAEMON_AGRO_HPP__ */
