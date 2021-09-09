#include "init.hpp"
#include <functional>

namespace Resource{

void init(engine& coap_engine, Device_List& list,
		Agro::share_ptr data_share,
		std::vector<engine::resource_node>& vresource) noexcept
{
	vresource.emplace_back("board",
		"title='Get board configuration'",
		nullptr,
		nullptr,
		std::bind(put_board_config_handler,
			std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
			std::ref(list), data_share));
	vresource.emplace_back("route",
			"title='Get devices route'",
			nullptr,
			nullptr,
			std::bind(put_route_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list), data_share));
	vresource.emplace_back("status",
			"title='Get devices status'",
			nullptr,
			nullptr,
			std::bind(Resource::put_status_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list), data_share));
	vresource.emplace_back("config",
			"title='Get devices configuration'",
			nullptr,
			nullptr,
			std::bind(put_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list), data_share));
	vresource.emplace_back("full_config",
			"title='Get all device configuration'",
			nullptr,
			nullptr,
			std::bind(put_full_config_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list), data_share));
	vresource.emplace_back("sensor",
			"title='Get device sensor data'",
			nullptr,
			nullptr,
			std::bind(put_sensor_data_handler,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
				std::ref(list), data_share));
	vresource.emplace_back("time",
			"title='local time'",
			get_time_handler);
	vresource.emplace_back("ota",
			"title='OTA image'",
			get_ota_handler);
	vresource.emplace_back("app",
			"title='Apps'",
			get_app_handler);
	vresource.emplace_back("info",
			"title='Device info'",
			nullptr,
			nullptr,
			std::bind(put_info_handler,
					std::placeholders::_1, std::placeholders::_2, std::placeholders::_3,
					data_share));

	for(auto& res : vresource)
	{
		coap_engine.root_node().add_child(res);
	}
}

}//Resource
