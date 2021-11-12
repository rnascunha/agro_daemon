#include "sqlite3.hpp"
#include "../error.hpp"
#include <string>
#include <vector>
#include <iostream>

sqlite3::sqlite3(const char* db_name, std::error_code& ec)
{
	int rc = open(db_name);
	if(rc != SQLITE_OK) ec = make_error_code(Error::open_db);
}

sqlite3::~sqlite3()
{
	close();
}

std::int64_t sqlite3::last_insert_rowid() const noexcept
{
	return sqlite3_last_insert_rowid(db_);
}

bool sqlite3::is_open() noexcept
{
	return db_ != NULL;
}

int sqlite3::open(const char* db_name) noexcept
{
	int rc = sqlite3_open(db_name, &db_);
	if(rc)
	{
		close();
	}
	return rc;
}

void sqlite3::close() noexcept
{
	if(!db_)
	{
		sqlite3_close(db_);
		db_ = NULL;
	}
}

int sqlite3::prepare(const char* sql_statment, statement& res, const char** tail /* = NULL */) noexcept
{
	return sqlite3_prepare_v2(db_, sql_statment, -1 , res.native_ptr(), tail);
}

int sqlite3::prepare(const char* sql_statment, int n_bytes, statement& res, const char** tail /* = NULL */) noexcept
{
	return sqlite3_prepare_v2(db_, sql_statment, n_bytes, res.native_ptr(), tail);
}

int sqlite3::prepare(const std::string_view& sql_statment, statement& res, const char** tail /* = NULL */) noexcept
{
	return sqlite3_prepare_v2(db_, sql_statment.data(), sql_statment.size(), res.native_ptr(), tail);
}

int sqlite3::exec(const char* statement,
		callback cb /* = NULL */,
		void* arg /* = NULL */,
		char** err /* = NULL */) noexcept
{
	return sqlite3_exec(db_, statement, cb, arg, err);
}

const char* sqlite3::error() noexcept
{
	return sqlite3_errmsg(db_);
}

sqlite3::statement::~statement()
{
	finalize();
}

void sqlite3::statement::finalize() noexcept
{
	if(stmt_ != NULL)
	{
		sqlite3_finalize(stmt_);
		stmt_ = NULL;
	}
}

int sqlite3::statement::step() noexcept
{
	return sqlite3_step(stmt_);
}

int sqlite3::statement::bind(int index, int value) noexcept
{
	return sqlite3_bind_int(stmt_, index, value);
}

int sqlite3::statement::bind(int index, unsigned value) noexcept
{
	return sqlite3_bind_int(stmt_, index, value);
}

int sqlite3::statement::bind(int index, long value) noexcept
{
	return sqlite3_bind_int64(stmt_, index, value);
}

int sqlite3::statement::bind(int index,
		std::string const& value,
		destructor_type destructor/* = static_destructor */) noexcept
{
	return sqlite3_bind_text(stmt_, index, value.data(), value.size(), destructor);
}

int sqlite3::statement::bind(int index, const void* data, int size, destructor_type destructor /* = static_destructor */) noexcept
{
	return sqlite3_bind_blob(stmt_, index, data, size, destructor);
}

int sqlite3::statement::bind(int index, binary const& data, destructor_type destructor /* = static_destructor */) noexcept
{
	return bind(index, data.data, data.size, destructor);
}

int sqlite3::statement::reset() noexcept
{
	return sqlite3_reset(stmt_);
}

int sqlite3::statement::clear_bidings() noexcept
{
	return sqlite3_clear_bindings(stmt_);
}

int sqlite3::statement::interger(int index) noexcept
{
	return sqlite3_column_int(stmt_, index);
}

long sqlite3::statement::long_interger(int index) noexcept
{
	return sqlite3_column_int64(stmt_, index);
}

std::string sqlite3::statement::text(int index) noexcept
{
	const unsigned char* t = sqlite3_column_text(stmt_, index);
	return {reinterpret_cast<const char*>(t),
						static_cast<std::size_t>(sqlite3_column_bytes(stmt_, index))};
}

const unsigned char* sqlite3::statement::text(int index, int& size) noexcept
{
	const unsigned char* val = sqlite3_column_text(stmt_, index);
	size = sqlite3_column_bytes(stmt_, index);
	return val;
}

const void* sqlite3::statement::blob(int index, int& size) noexcept
{
	const void* val = sqlite3_column_blob(stmt_, index);
	size = sqlite3_column_bytes(stmt_, index);
	return val;
}

std::vector<unsigned char> sqlite3::statement::blob(int index) noexcept
{
	const unsigned char* val = static_cast<const unsigned char*>(sqlite3_column_blob(stmt_, index));
	return {val,
		val + static_cast<std::size_t>(sqlite3_column_bytes(stmt_, index))};
}

sqlite3_stmt* sqlite3::statement::native() noexcept
{
	return stmt_;
}

sqlite3_stmt** sqlite3::statement::native_ptr() noexcept
{
	return &stmt_;
}
