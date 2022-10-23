#include "../../instance/agro.hpp"
#include "../../coap_engine.hpp"
#include "../list.hpp"

#include <sstream>
#include <iomanip>

namespace Agro{
namespace Device{
namespace Resource{

void post_job_handler(engine::message const& request,
					engine::response& response, void*,
					instance& instance) noexcept
{
	CoAP::Message::Option::option op;
	Agro::Device::Device* dev;
	if(!instance.process_device_request(request, &dev, op))
	{
		//Missing host... nothing to do
		//(reality is that without host message wouldn't even get here)
		return;
	}

	int index = *static_cast<int const*>(request.payload);

	std::stringstream ss;
	Agro::Notify::general_type type;
	if(index < 0)
	{
		//No jobs is running at device
		ss << "No job running";
		type = Agro::Notify::general_type::job_stopped;
	}
	else
	{
		type = Agro::Notify::general_type::job_running;
		//Job is running at device
		if(index < static_cast<int>(dev->jobs().size()))
		{
			//We can check which job is running
			auto const& job = dev->jobs()[index];
			ss << "Job running [" << index << "]: "
				<< std::setw(2) << std::setfill('0')
				<< +job.time_before.hour << ":"
				<< std::setw(2) << std::setfill('0')
				<< +job.time_before.minute << "-"
				<< std::setw(2) << std::setfill('0')
				<< +job.time_after.hour << ":"
				<< std::setw(2) << std::setfill('0')
				<< +job.time_after.minute << " "
				<< std::setw(1)
				<< static_cast<int>(job.day_of_week) << " / priority = " << +job.priority << " | "
				<< job.app_name << "[" << job.argument << "]";
		}
		else
		{
			//We don't know which job is running
			ss << "Job '" << index << "' is runnning";
		}
	}

	instance.share()->write_all_policy(Agro::Authorization::rule::view_device,
						instance.make_report(Agro::Message::report_type::info,
								dev->mac(),
								ss.str(),
								"",
								User::root_id));

	std::stringstream temp;
	temp << "[" << dev->mac().to_string() << "] " << ss.rdbuf();
	instance.notify_all_policy(Agro::Authorization::rule::view_device, type, temp.str());

	if(request.mtype == CoAP::Message::type::confirmable)
	{
		CoAP::Message::Option::node host_op{op};
		response
			.code(CoAP::Message::code::changed)
			.add_option(host_op)
			.serialize();
	}
}

}//Resource
}//Device
}//Agro
