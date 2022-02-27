#include "types.hpp"
#include "../../websocket/types.hpp"
#include "../message/device.hpp"

namespace Agro{
namespace Device{
namespace Request{

static void process_jobs(Agro::Device::Device_List& device_list,
		mesh_addr_t const& host,
		std::uint8_t const* data, std::size_t size,
		Agro::websocket_ptr ws) noexcept
{
	auto* dev = device_list[host];
	if(!dev)
	{
		tt::error("[job] Device '%s' not found", host.to_string().c_str());
		return;
	}
	dev->jobs(data, size);
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Message::device_jobs_to_json(*dev)));
}

static void send_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		tt::error("[send job] Error sending jobs [%s]", p.c_str());
		ws->write(instance.make_report(Agro::Message::report_type::error,
							host, p, "PUT job", ws->user().id()));
		return;
	}
	process_jobs(instance.device_list(), host,
			static_cast<std::uint8_t const*>(request.payload), request.payload_len, ws);
}

static void get_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		tt::error("[get job] Error sending jobs [%s]", p.c_str());
		ws->write(instance.make_report(Agro::Message::report_type::error,
							host, p, "GET job", ws->user().id()));
		return;
	}
	process_jobs(instance.device_list(), host,
				static_cast<std::uint8_t const*>(response.payload), response.payload_len, ws);
}

static void delete_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		request_type,
		CoAP::Message::message const&,
		CoAP::Message::message const&,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	process_jobs(instance.device_list(), host, static_cast<std::uint8_t const*>(nullptr), 0, ws);
}

static std::size_t send_job_payload(
		rapidjson::Document const& doc,
		void* buf,
		std::size_t,
		instance&,
		std::error_code& ec)
{
	if(!doc.HasMember("data") || !doc["data"].IsObject())
	{
		ec = make_error_code(Error::missing_field);
		return 0;
	}
	auto const& data = doc["data"].GetObject();

	if(!data.HasMember("jobs") || !data["jobs"].IsArray())
	{
		ec = make_error_code(Error::missing_field);
		return 0;
	}

	std::size_t offset = 0;
	for(auto const& job : data["jobs"].GetArray())
	{
		if(!job.IsObject())
		{
			continue;
		}

		if(!job.HasMember("begin") || !job["begin"].IsObject())
		{
			continue;
		}
		auto const& begin = job["begin"].GetObject();

		if((!begin.HasMember("hour") || !begin["hour"].IsUint())
			|| (!begin.HasMember("minute") || !begin["minute"].IsUint()))
		{
			continue;
		}

		if(!job.HasMember("end") || !job["end"].IsObject())
		{
			continue;
		}
		auto const& end = job["end"].GetObject();

		if((!end.HasMember("hour") || !end["hour"].IsUint())
			|| (!end.HasMember("minute") || !end["minute"].IsUint()))
		{
			continue;
		}

		if(!job.HasMember("dow") || !job["dow"].IsUint())
		{
			continue;
		}

		if(!job.HasMember("priority") || !job["priority"].IsUint())
		{
			continue;
		}

		if(!job.HasMember("exec") || !job["exec"].IsString())
		{
			continue;
		}

		if(!job.HasMember("arg") || !job["arg"].IsInt())
		{
			continue;
		}

		/**
		 * Scheduler
		 */
		job_packet packet;
		packet.be_time_hour = begin["hour"].GetUint();
		packet.be_time_minute = begin["minute"].GetUint();
		packet.af_time_hour = end["hour"].GetUint();
		packet.af_time_minute = end["minute"].GetUint();
		packet.day_of_week = static_cast<dow>(job["dow"].GetUint());
		packet.priority = job["priority"].GetUint();

		std::memcpy(static_cast<std::uint8_t*>(buf) + offset, &packet, sizeof(packet));
		offset += sizeof(packet);

		/**
		 * Executor
		 */
		job_packet_executor packet_exec;
		packet_exec.arg = job["arg"].GetInt();
		std::memcpy(packet_exec.exec, job["exec"].GetString(), app_name_max_size);
		packet_exec.exec[app_name_max_size - 1] = '\0';

		std::memcpy(static_cast<std::uint8_t*>(buf) + offset, &packet_exec, sizeof(job_packet_executor));
		offset += sizeof(job_packet_executor);
	}

	return offset;
}

static request_message const req_send_job = {
	CoAP::Message::code::put,
	{
		{CoAP::Message::Option::code::uri_path, "job"},
	},
	CoAP::Message::type::confirmable,
	send_job_payload
};

static request_message const req_get_job = {
	CoAP::Message::code::get,
	{
		{CoAP::Message::Option::code::uri_path, "job"},
	}
};

static request_message const req_del_job = {
	CoAP::Message::code::cdelete,
	{
		{CoAP::Message::Option::code::uri_path, "job"},
	}
};

extern constexpr const request_config send_job = {
	{request_type::send_job, "send_job"},
	&req_send_job,
	send_job_response
};
extern constexpr const request_config get_job = {
	{request_type::get_job, "get_job"},
	&req_get_job,
	get_job_response
};
extern constexpr const request_config delete_job = {
	{request_type::delete_job, "del_job"},
	&req_del_job,
	delete_job_response
};

}//Request
}//Device
}//Agro
