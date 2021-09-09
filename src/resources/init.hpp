#ifndef AGRO_DAEMON_RESOURCE_INIT_HPP__
#define AGRO_DAEMON_RESOURCE_INIT_HPP__

#include <vector>
#include <memory>
#include "../device/list.hpp"
#include "../websocket/types.hpp"

namespace Resource{

void init(engine&,
		Device_List&,
		Agro::share_ptr,
		std::vector<engine::resource_node>&) noexcept;

void put_route_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void put_status_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void put_board_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void put_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void put_full_config_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void put_sensor_data_handler(engine::message const& request,
								engine::response& response, void*,
								Device_List& device_list,
								Agro::share_ptr) noexcept;
void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
void get_ota_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
void get_app_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
void put_info_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::share_ptr) noexcept;

}//Resource

#endif /* AGRO_DAEMON_RESOURCE_INIT_HPP__ */
