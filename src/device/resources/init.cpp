#include "init.hpp"
#include <functional>

namespace Agro{
namespace Device{
namespace Resource{

void init(engine& coap_engine,
		Agro::instance& instance,
		std::vector<engine::resource_node>& vresource) noexcept
{
	vresource.emplace_back("board",
		"title='Get board configuration'",
		nullptr,
		nullptr,
		std::bind(put_board_config_handler,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::ref(instance)));
	vresource.emplace_back("route",
			"title='Get devices route'",
			nullptr,
			nullptr,
			std::bind(put_route_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(instance)));
	vresource.emplace_back("config",
			"title='Get devices configuration'",
			nullptr,
			nullptr,
			std::bind(put_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(instance)));
	vresource.emplace_back("full_config",
			"title='Get all device configuration'",
			nullptr,
			nullptr,
			std::bind(put_full_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(instance)));
	vresource.emplace_back("sensors",
				"title='Get device sensor data'",
				nullptr,
				nullptr,
				std::bind(put_sensors_data_handler,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
					std::ref(instance)));
	vresource.emplace_back("time",
			"title='local time'",
			get_time_handler);
	vresource.emplace_back("ota",
			"title='OTA image'",
			std::bind(get_ota_handler,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
					std::ref(instance)));
	vresource.emplace_back("app",
			"title='Apps'",
			std::bind(get_app_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(instance)));
	vresource.emplace_back("info",
			"title='Device info'",
			nullptr,
			nullptr,
			std::bind(put_info_handler,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
					std::ref(instance)));

	for(auto& res : vresource)
	{
		coap_engine.root_node().add_child(res);
	}
}

}//Resource
}//Device
}//Agro
