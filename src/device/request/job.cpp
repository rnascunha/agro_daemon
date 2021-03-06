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
	dev->jobs(data, size);
	ws->write_all_policy(Agro::Authorization::rule::view_device,
			std::make_shared<std::string>(Message::device_jobs_to_json(*dev)));
}

static void send_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const& request,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Update JOB error[" << response.payload_len << "]: " << p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::error, host, p.c_str())
//		);
		return;
	}
	process_jobs(instance.device_list(), host,
			static_cast<std::uint8_t const*>(request.payload), request.payload_len, ws);
}

static void get_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
		CoAP::Message::message const&,
		CoAP::Message::message const& response,
		CoAP::Transmission::status_t,
		Agro::instance& instance,
		Agro::websocket_ptr ws) noexcept
{
	if(CoAP::Message::is_error(response.mcode))
	{
		std::string p{static_cast<const char*>(response.payload), response.payload_len};
		std::cerr << "Get JOB error[" << response.payload_len << "]: " << p << "\n";
//		ws->write_all(
//				::Message::make_info(::Message::info::error, host, p.c_str())
//		);
		return;
	}
	process_jobs(instance.device_list(), host,
			static_cast<std::uint8_t const*>(response.payload), response.payload_len, ws);
}

static void delete_job_response(
		engine::endpoint const&,
		mesh_addr_t const& host,
		type,
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
	if(doc.HasMember("payload") && doc["payload"].IsArray())
	{
		std::size_t offset = 0;
		rapidjson::Value const& sch_arr = doc["payload"];
		for(auto const& sch : sch_arr.GetArray())
		{
			std::uint8_t pay[7];

			if(sch.HasMember("init") && sch["init"].IsObject())
			{
				rapidjson::Value const& init = sch["init"];
				if(init.HasMember("hour") && init["hour"].IsUint() &&
					init.HasMember("minute") && init["minute"].IsUint())
				{
					pay[0] = static_cast<std::uint8_t>(init["hour"].GetUint());
					pay[1] = static_cast<std::uint8_t>(init["minute"].GetUint());
				} else {
					ec = make_error_code(Error::ill_formed);
					return 0;
				}
			}
			else return 0;
			if(sch.HasMember("end") && sch["end"].IsObject())
			{
				rapidjson::Value const& end = sch["end"];
				if(end.HasMember("hour") && end["hour"].IsUint() &&
					end.HasMember("minute") && end["minute"].IsUint())
				{
					pay[2] = static_cast<std::uint8_t>(end["hour"].GetUint());
					pay[3] = static_cast<std::uint8_t>(end["minute"].GetUint());
				} else {
					ec = make_error_code(Error::ill_formed);
					return 0;
				}
			}
			else {
				ec = make_error_code(Error::ill_formed);
				return 0;
			}
			if(sch.HasMember("dow") && sch["dow"].IsUint())
			{
				pay[4] = sch["dow"].GetUint();
			} else {
				ec = make_error_code(Error::ill_formed);
				return 0;
			}
			if(sch.HasMember("priority") && sch["priority"].IsUint())
			{
				pay[5] = sch["priority"].GetUint();
			} else {
				ec = make_error_code(Error::ill_formed);
				return 0;
			}
			if(sch.HasMember("active") && sch["active"].IsUint())
			{
				pay[6] = sch["active"].GetUint();
			} else {
				ec = make_error_code(Error::ill_formed);
				return 0;
			}

			std::memcpy(static_cast<std::uint8_t*>(buf) + offset, pay, 7);
			offset += 7;
		}
		return offset;
	}
	ec = make_error_code(Error::ill_formed);
	return 0;
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
	type::send_job,
	"send_job",
	&req_send_job,
	send_job_response
};
extern constexpr const request_config get_job = {
	type::get_job,
	"get_job",
	&req_get_job,
	get_job_response
};
extern constexpr const request_config delete_job = {
	type::delete_job,
	"del_job",
	&req_del_job,
	delete_job_response
};

}//Request
}//Device
}//Agro
