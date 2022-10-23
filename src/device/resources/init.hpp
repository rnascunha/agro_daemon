#ifndef AGRO_DAEMON_DEVICE_RESOURCE_INIT_HPP__
#define AGRO_DAEMON_DEVICE_RESOURCE_INIT_HPP__

#include <vector>
#include <memory>

#include "../../instance/agro.hpp"
#include "../../websocket/types.hpp"

namespace Agro{
namespace Device{
namespace Resource{

void init(engine&,
		Agro::instance&,
		std::vector<engine::resource_node>&) noexcept;

void put_route_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_status_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_board_config_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_config_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_full_config_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void get_time_handler(engine::message const& request,
								engine::response& response, void*) noexcept;
void get_ota_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void get_app_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void post_app_handler(engine::message const& request,
						engine::response& response, void*,
						instance& instance) noexcept;
void put_info_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_report_handler(engine::message const& request,
								engine::response& response, void*,
								Agro::instance&) noexcept;
void put_sensors_data_handler(engine::message const& request,
					engine::response& response, void*,
					Agro::instance& instance) noexcept;
void post_job_handler(engine::message const& request,
					engine::response& response, void*,
					instance& instance) noexcept;

}//Resource
}//Device
}//Agro

#endif /* AGRO_DAEMON_DEVICE_RESOURCE_INIT_HPP__ */
