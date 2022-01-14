#ifndef AGRO_DAEMON_WEBSOCKET_SHARE_HPP__
#define AGRO_DAEMON_WEBSOCKET_SHARE_HPP__

#include <unordered_set>
#include <mutex>

#include <functional>
#include <vector>
#include "../user/policy.hpp"

namespace Agro{

class instance;

}

template<typename Session>
class share{
	public:
		share(Agro::instance& instance)
			: instance_(instance){}

		void join(Session* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.insert(sh);
		}

		void leave(Session* sh) noexcept{
			std::lock_guard<std::mutex> lock(mutex_);
			share_.erase(sh);
		}

		void exec_to_all(std::function<void(Session*)> func) noexcept
		{
			std::vector<std::weak_ptr<Session>> v;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				v.reserve(share_.size());
				for(auto p : share_)
					v.emplace_back(p->weak_from_this());
			}

			for(auto const& wp : v){
				if(auto sp = wp.lock()){
					func(sp.get());
				}
			}
		}

		void exec_to_all(std::function<void(void)> func) noexcept
		{
			std::vector<std::weak_ptr<Session>> v;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				v.reserve(share_.size());
				for(auto p : share_)
					v.emplace_back(p->weak_from_this());
			}

			for(auto const& wp : v){
				if(auto sp = wp.lock()){
					sp->func();
				}
			}
		}

		void exec_policy(Agro::Authorization::rule rule,
				std::function<void(Session*)> func) noexcept
		{
			std::vector<std::weak_ptr<Session>> v;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				v.reserve(share_.size());
				for(auto p : share_)
				{
					if(Agro::Authorization::can(p->user(), rule))
						v.emplace_back(p->weak_from_this());
				}
			}

			for(auto const& wp : v)
			{
				if(auto sp = wp.lock()){
					func(sp.get());
				}
			}
		}

		void write_all(std::shared_ptr<std::string> data) noexcept
		{
			std::vector<std::weak_ptr<Session>> v;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				v.reserve(share_.size());
				for(auto p : share_)
					v.emplace_back(p->weak_from_this());
			}

			for(auto const& wp : v){
				if(auto sp = wp.lock()){
					sp->write(data);
				}
			}
		}

		void write_all_policy(Agro::Authorization::rule rule,
						std::shared_ptr<std::string const> const data) noexcept
		{
			std::vector<std::weak_ptr<Session>> v;
			{
				std::lock_guard<std::mutex> lock(mutex_);
				v.reserve(share_.size());
				for(auto p : share_)
				{
					if(Agro::Authorization::can(p->user(), rule))
						v.emplace_back(p->weak_from_this());
				}
			}

			for(auto const& wp : v)
			{
				if(auto sp = wp.lock()){
					sp->write(data);
				}
			}
		}

		Agro::instance const& instance() const noexcept
		{
			return instance_;
		}

		Agro::instance& instance() noexcept
		{
			return instance_;
		}
	private:
		std::mutex mutex_;
		std::unordered_set<Session*> share_;
		Agro::instance& instance_;
};

#endif /* AGRO_DAEMON_WEBSOCKET_SHARE_HPP__ */
