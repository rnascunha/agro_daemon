#ifndef AGRO_DAEMON_DB_SQLITE3_HPP__
#define AGRO_DAEMON_DB_SQLITE3_HPP__

#include <system_error>
#include <sqlite3.h>
#include <string_view>
#include <string>
#include <vector>

class sqlite3{
	public:
		using callback = int(*)(void*, int, char**, char**);
		using destructor_type = sqlite3_destructor_type;
		static constexpr const destructor_type static_destructor = SQLITE_STATIC;
//		static constexpr const destructor_type transient_destructor = SQLITE_TRANSIENT;

		struct binary{
			const void* data;
			int size;
		};

		class statement{
			public:
				~statement();

				void finalize() noexcept;
				int step() noexcept;

				int bind(int, int) noexcept;
				int bind(int, unsigned) noexcept;
				int bind(int, long) noexcept;
				int bind(int, float) noexcept;
				int bind(int, double) noexcept;

				int bind(int, std::string const&, destructor_type = static_destructor) noexcept;
				int bind(int, const void*, int, destructor_type = static_destructor) noexcept;
				int bind(int, binary const& data, destructor_type = static_destructor) noexcept;

				template<typename ...Args>
				int bind_all(Args&&...) noexcept;

				int reset() noexcept;
				int clear_bidings() noexcept;

				int integer(int) noexcept;
				long long_integer(int) noexcept;

				float real(int) noexcept;
				double long_real(int) noexcept;

				std::string text(int) noexcept;
				const unsigned char* text(int, int& size) noexcept;
				const void* blob(int, int& size) noexcept;
				std::vector<unsigned char> blob(int) noexcept;

				sqlite3_stmt* native() noexcept;
				sqlite3_stmt** native_ptr() noexcept;
			private:
				sqlite3_stmt* stmt_ = NULL;

				template<typename Arg>
				int bind_impl(int index, Arg&& arg) noexcept;
				template<typename Arg, typename ...Args>
				int bind_impl(int index, Arg&& arg, Args&& ...args) noexcept;
		};

		sqlite3(const char* db_name, std::error_code&);
		~sqlite3();

		std::int64_t last_insert_rowid() const noexcept;

		bool is_open() noexcept;

		int open(const char*) noexcept;
		void close() noexcept;

		int prepare(const char* sql_statment, statement& res, const char** tail = nullptr) noexcept;
		int prepare(const char* sql_statment, int n_bytes, statement& res, const char** tail = nullptr) noexcept;
		int prepare(const std::string_view& sql_statment, statement& res, const char** tail = nullptr) noexcept;

		template<typename ...Args>
		int prepare_bind(const std::string_view& sql_statement, statement& res, Args&& ...args) noexcept;

		int exec(const char* statement, callback = NULL,
				void* = nullptr, char** err = nullptr) noexcept;

		const char* error() noexcept;
	private:
		sqlite3 *db_ = nullptr;

		template<typename Arg>
		int prepare_bind_impl(statement& res, int index, Arg&& arg) noexcept;
		template<typename Arg, typename ...Args>
		int prepare_bind_impl(statement& res, int index, Arg&& arg, Args&& ...args) noexcept;
};

#include "impl/sqlite3_impl.hpp"

#endif /* AGRO_DAEMON_DB_SQLITE3_HPP__ */
