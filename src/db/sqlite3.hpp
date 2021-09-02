#ifndef AGRO_DAEMON_DB_SQLITE3_HPP__
#define AGRO_DAEMON_DB_SQLITE3_HPP__

#include <system_error>
#include <sqlite3.h>
#include <string>
#include <vector>

class sqlite3{
	public:
		using callback = int(*)(void*, int, char**, char**);
		using destructor_type = sqlite3_destructor_type;
		static constexpr const destructor_type static_destructor = SQLITE_STATIC;
//		static constexpr const destructor_type transient_destructor = SQLITE_TRANSIENT;

		class statement{
			public:
				~statement();

				void finalize() noexcept;
				int step() noexcept;

				int bind(int, int) noexcept;
				int bind(int, std::string const&, destructor_type = static_destructor) noexcept;
				int bind(int, const void*, int, destructor_type = static_destructor) noexcept;

				int reset() noexcept;

				int interger(int) noexcept;
				std::string text(int) noexcept;
				const unsigned char* text(int, int& size) noexcept;
				const void* blob(int, int& size) noexcept;
				std::vector<unsigned char> blob(int) noexcept;

				sqlite3_stmt* native() noexcept;
				sqlite3_stmt** native_ptr() noexcept;
			private:
				sqlite3_stmt* stmt_ = NULL;
		};

		sqlite3(const char* db_name, std::error_code&);
		~sqlite3();

		bool is_open() noexcept;

		int open(const char*) noexcept;
		void close() noexcept;

		int prepare(const char* sql_statment, statement& res, const char** tail = NULL) noexcept;
		int prepare(const char* sql_statment, int n_bytes, statement& res, const char** tail = NULL) noexcept;
		int prepare(const std::string_view& sql_statment, statement& res, const char** tail = NULL) noexcept;

		int exec(const char* statement, callback = NULL,
				void* = NULL, char** err = NULL) noexcept;

		const char* error() noexcept;
	private:
		sqlite3 *db_ = NULL;
};

#endif /* AGRO_DAEMON_DB_SQLITE3_HPP__ */
