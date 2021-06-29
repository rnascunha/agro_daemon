#include "init.hpp"
#include <functional>

namespace Resource{

void init(engine& coap_engine, Device_List& list, std::vector<engine::resource_node>& vresource) noexcept
{
	vresource.emplace_back("board",
		"Get board configuration",
		nullptr,
		nullptr,
		std::bind(put_board_config_handler,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::ref(list)));
	vresource.emplace_back("route",
			"Get devices route",
			nullptr,
			nullptr,
			std::bind(put_route_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list)));
	vresource.emplace_back("status",
			"Get devices status",
			nullptr,
			nullptr,
			std::bind(Resource::put_status_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list)));
	vresource.emplace_back("config",
			"Get devices configuration",
			nullptr,
			nullptr,
			std::bind(put_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list)));
	vresource.emplace_back("full_config",
			"Get all device configuration",
			nullptr,
			nullptr,
			std::bind(put_full_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list)));

	for(auto& res : vresource)
	{
		coap_engine.root_node().add_child(res);
	}
}

}//Resource
