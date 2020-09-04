/* mysql.h */
#ifndef MYSQL_H
#define MYSQL_H

#include "core/reference.h"

#include <mysql_error.h>
#include <mysql_driver.h>
#include <mysql_connection.h>

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <cppconn/metadata.h>
#include <cppconn/connection.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/prepared_statement.h>

#pragma once
//#define CPPCONN_PUBLIC_FUNC


using namespace std;
using namespace sql::mysql;


class MySQL : public Reference {
	GDCLASS(MySQL, Reference);


private:
	//---Vars
	sql::ConnectOptionsMap connection_properties;
	int afectedrows;
	const Array emptyarray;
	sql::mysql::MySQL_Driver *driver;
	std::shared_ptr <sql::Connection> con;

	//---Helpers
	bool check(int what);
	String sql2String(const sql::SQLString &p_str);
	void print_SQLException(sql::SQLException &e);
	void print_runtime_error(std::runtime_error &e);
	bool is_mysql_time(const String &time);
	void determine_datatype(std::shared_ptr<sql::PreparedStatement> prep_stmt, const Array &prep_val);
	Array format_time(const String &str, bool return_string);
	//---Database
	std::shared_ptr<sql::Connection> connection(int what);
	Array make_query(const String &p_SQLquery, int type, const Array &prep_val, bool return_string = false);

	//---Const
	enum { ACT_DO, ACT_CHECK, ACT_CLOSE};
	enum { 
		FUNC_NAME, 
		FUNC_TYPE, 
		FUNC_DICT, 
		FUNC_ARRAY, 
		FUNC_EXEC,
		FUNC_NAME_PREP, 
		FUNC_TYPE_PREP, 
		FUNC_DICT_PREP, 
		FUNC_ARRAY_PREP, 
		FUNC_EXEC_PREP,
	};


protected:
	static void _bind_methods();


public:
	//--- Connection Managers
	bool connection_start();
	bool connection_check();
	bool connection_close();
	//--- Credentials
	void set_credentials(const String &p_host, const String &p_user, const String &p_pass);
	void set_client_options(const String &p_option, const String &p_value);
	String get_client_options(const String &p_option);
	//--- Database
	void set_database(const String &p_database);
	String get_database();
	//--- Prepared Query
	int prep_execute(const String &p_SQLquery, const Array &prep_val);
	Array prep_fetch_dictionary(const String &p_SQLquery, const Array &prep_val, bool return_string = false);
	Array prep_fetch_array(const String &p_SQLquery, const Array &prep_val, bool return_string = false);
	Array prep_get_columns_names(const String &p_sql_query, const Array &prep_val);
	Array prep_get_columns_types(const String &p_sql_query, const Array &prep_val);
	//---  Query
	int query_execute(const String &p_SQLquery);
	Array query_fetch_dictionary(const String &p_SQLquery, bool return_string = false);
	Array query_fetch_array(const String &p_SQLquery, bool return_string = false);
	Array query_get_columns_names(const String &p_sql_query);
	Array query_get_columns_types(const String &p_sql_query);

	MySQL();
	~MySQL();
};

#endif	// MYSQL_H

	//Savepoint *savept;
	//int tipo = p_value.get_type();
	//tipo == Variant::INT
	//if (pstmt->getMoreResults())
