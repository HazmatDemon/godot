/*  MySQL.cpp */

#include "mysql.h"
#include <memory>

//-------------- Connection Managers
bool MySQL::connection_start() {
	return check(ACT_DO);
}

bool MySQL::connection_check() {
	return check(ACT_CHECK);
}

bool MySQL::connection_close() {
	return check(ACT_CLOSE);
}

//-------------- Query
Array MySQL::query_fetch_dictionary(const String &p_SQLquery, bool return_string) {
	return make_query(p_SQLquery, FUNC_DICT, emptyarray, return_string);
}

Array MySQL::query_fetch_array(const String &p_SQLquery, bool return_string) {
	return make_query(p_SQLquery, FUNC_ARRAY, emptyarray, return_string);
}

Array MySQL::query_get_columns_types(const String &p_SQLquery) {
	return make_query(p_SQLquery, FUNC_TYPE, emptyarray, true);
}

Array MySQL::query_get_columns_names(const String &p_SQLquery) {
	return make_query(p_SQLquery, FUNC_NAME, emptyarray, true);
}

//-------------- Prepared Query
Array MySQL::prep_fetch_dictionary(const String &p_SQLquery, const Array &prep_val, bool return_string) {
	return make_query(p_SQLquery, FUNC_DICT_PREP, prep_val, return_string);
}

Array MySQL::prep_fetch_array(const String &p_SQLquery, const Array &prep_val, bool return_string) {
	return make_query(p_SQLquery, FUNC_ARRAY_PREP, prep_val, return_string);
}

Array MySQL::prep_get_columns_types(const String &p_SQLquery, const Array &prep_val) {
	return make_query(p_SQLquery, FUNC_TYPE_PREP, prep_val, true);
}

Array MySQL::prep_get_columns_names(const String &p_SQLquery, const Array &prep_val) {
	return make_query(p_SQLquery, FUNC_NAME_PREP, prep_val, true);
}

void MySQL::set_credentials(const String &p_host, const String &p_user, const String &p_pass) {
	connection_properties["hostName"] = p_host.utf8().get_data();
	connection_properties["userName"] = p_user.utf8().get_data();
	connection_properties["password"] = p_pass.utf8().get_data();
}

void MySQL::set_client_options(const String &p_option, const String &p_value) {
	shared_ptr<sql::Connection> con(connection(ACT_DO));
	sql::SQLString option = p_option.utf8().get_data();
	sql::SQLString value = p_value.utf8().get_data();
	con->setClientOption(option, value);
}

String MySQL::get_client_options(const String &p_option) {
	shared_ptr<sql::Connection> con(connection(ACT_DO));
	sql::SQLString option = p_option.utf8().get_data();
	return sql2String(con->getClientOption(option));
}

int MySQL::query_execute(const String &p_SQLquery) {
	make_query(p_SQLquery, FUNC_EXEC, emptyarray, false);
	int rows = afectedrows;
	afectedrows = 0;
	return rows;
}

int MySQL::prep_execute(const String &p_SQLquery, const Array &prep_val) {
	make_query(p_SQLquery, FUNC_EXEC_PREP, prep_val, false);
	int rows = afectedrows;
	afectedrows = 0;
	return rows;
}

//-------------- Database
String MySQL::get_database() {
	shared_ptr<sql::Connection> con(connection(ACT_DO));
	if (con != NULL) {
		return sql2String(con->getSchema());
	} else {
		return (String) "Invalid Connection!";
	}
}

void MySQL::set_database(const String &p_database) {
	sql::SQLString database = p_database.utf8().get_data();
	if (database != "") {
		shared_ptr<sql::Connection> con(connection(ACT_DO));
		if (con != NULL) {
			con->setSchema(database);
		} else {
			connection_properties["schema"] = database;
		}
	}
}

Array MySQL::make_query(const String &p_SQLquery, int type, const Array &prep_val, bool return_string) {
	sql::SQLString SQLquery = p_SQLquery.utf8().get_data();
	Array ret;
	shared_ptr<sql::Connection> con(connection(ACT_DO));
	shared_ptr<sql::Statement> stmt;
	shared_ptr<sql::PreparedStatement> prep_stmt;
	shared_ptr<sql::ResultSet> res;
	sql::ResultSetMetaData *res_meta;

	try {
		switch (type) {
			case FUNC_NAME: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				res.reset(prep_stmt->executeQuery());
				res_meta = res->getMetaData();

				for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
					ret.push_back(sql2String(res_meta->getColumnName(i)));
				}
			} break;
			case FUNC_TYPE: {
				stmt.reset(con->createStatement());
				res.reset(stmt->executeQuery(SQLquery));
				res_meta = res->getMetaData();

				for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
					ret.push_back(sql2String(res_meta->getColumnTypeName(i)));
				}
			} break;
			case FUNC_DICT: {
				stmt.reset(con->createStatement());
				res.reset(stmt->executeQuery(SQLquery));
				res_meta = res->getMetaData();
				while (res->next()) {
					Dictionary row;
					for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {

						if (return_string) {
							row[sql2String(res_meta->getColumnName(i))] = sql2String(res->getString(i));
						} else {
							int g_type = res_meta->getColumnType(i);

							switch (g_type) {
								case sql::DataType::BIT:
								case sql::DataType::TINYINT:
								case sql::DataType::SMALLINT:
								case sql::DataType::MEDIUMINT:
								case sql::DataType::INTEGER:
								case sql::DataType::BIGINT: {
									row[sql2String(res_meta->getColumnName(i))] = res->getInt(i);
								} break;
								case sql::DataType::REAL:
								case sql::DataType::DOUBLE:
								case sql::DataType::DECIMAL:
								case sql::DataType::NUMERIC: {
									row[sql2String(res_meta->getColumnName(i))] = (double) res->getDouble(i);
								} break;
								case sql::DataType::DATE:
								case sql::DataType::TIME:
								case sql::DataType::TIMESTAMP:
								case sql::DataType::YEAR: {
									row[sql2String(res_meta->getColumnName(i))] = format_time(((sql2String(res->getString(i))).utf8().get_data()), false);
								} break;
								default: {
									row[sql2String(res_meta->getColumnName(i))] = sql2String(res->getString(i));
								} break;
							}
						}
					}
					ret.push_back(row);
				}
			} break;
			case FUNC_ARRAY: {
				stmt.reset(con->createStatement());
				res.reset(stmt->executeQuery(SQLquery));
				res_meta = res->getMetaData();
				while (res->next()) {
					Array line;
					for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
						
						if (return_string) {
							line.push_back((sql2String(res->getString(i))));
						} else {
							int g_type = res_meta->getColumnType(i);

							switch (g_type) {
								case sql::DataType::BIT:
								case sql::DataType::TINYINT:
								case sql::DataType::SMALLINT:
								case sql::DataType::MEDIUMINT:
								case sql::DataType::INTEGER:
								case sql::DataType::BIGINT: {
									line.push_back(res->getInt(i));
								} break;
								case sql::DataType::REAL:
								case sql::DataType::DOUBLE:
								case sql::DataType::DECIMAL:
								case sql::DataType::NUMERIC: {
									line.push_back((double)res->getDouble(i));
								} break;
								case sql::DataType::DATE:
								case sql::DataType::TIME:
								case sql::DataType::TIMESTAMP:
								case sql::DataType::YEAR: {
									line.push_back(format_time(((sql2String(res->getString(i))).utf8().get_data()), false));
								} break;
								default: {
									line.push_back(sql2String(res->getString(i)));
								} break;
							}
						}
					}
					ret.push_back(line);
				}
			} break;
			case FUNC_EXEC: {
				stmt.reset(con->createStatement());
				afectedrows = stmt->executeUpdate(SQLquery);
			} break;
			case FUNC_NAME_PREP: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				res.reset(prep_stmt->executeQuery());
				res_meta = res->getMetaData();

				for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
					ret.push_back(sql2String(res_meta->getColumnName(i)));
				}
			} break;
			case FUNC_TYPE_PREP: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				res.reset(prep_stmt->executeQuery());
				res_meta = res->getMetaData();

				for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
					ret.push_back(sql2String(res_meta->getColumnTypeName(i)));
				}
			} break;
			case FUNC_DICT_PREP: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				res.reset(prep_stmt->executeQuery());
				res_meta = res->getMetaData();

				while (res->next()) {
					Dictionary row;
					for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {

						if (return_string) {
							row[sql2String(res_meta->getColumnName(i))] = sql2String(res->getString(i));
						} else {
							int g_type = res_meta->getColumnType(i);

							switch (g_type) {
								case sql::DataType::BIT:
								case sql::DataType::TINYINT:
								case sql::DataType::SMALLINT:
								case sql::DataType::MEDIUMINT:
								case sql::DataType::INTEGER:
								case sql::DataType::BIGINT: {
									row[sql2String(res_meta->getColumnName(i))] = res->getInt(i);
								} break;
								case sql::DataType::REAL:
								case sql::DataType::DOUBLE:
								case sql::DataType::DECIMAL:
								case sql::DataType::NUMERIC: {
									row[sql2String(res_meta->getColumnName(i))] = (double)res->getDouble(i);
								} break;
								case sql::DataType::DATE:
								case sql::DataType::TIME:
								case sql::DataType::TIMESTAMP:
								case sql::DataType::YEAR: {
									row[sql2String(res_meta->getColumnName(i))] = format_time(((sql2String(res->getString(i))).utf8().get_data()), false);
								} break;
								default: {
									row[sql2String(res_meta->getColumnName(i))] = sql2String(res->getString(i));
								} break;
							}
						}
					}
					ret.push_back(row);
				}
			} break;
			case FUNC_ARRAY_PREP: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				res.reset(prep_stmt->executeQuery());
				res_meta = res->getMetaData();
				while (res->next()) {
					Array line;
					for (uint8_t i = 1; i <= res_meta->getColumnCount(); i++) {
						
						if (return_string) {
							line.push_back((sql2String(res->getString(i))));
						} else {
							int g_type = res_meta->getColumnType(i);

							switch (g_type) {
								case sql::DataType::BIT:
								case sql::DataType::TINYINT:
								case sql::DataType::SMALLINT:
								case sql::DataType::MEDIUMINT:
								case sql::DataType::INTEGER:
								case sql::DataType::BIGINT: {
									line.push_back(res->getInt(i));
								} break;
								case sql::DataType::REAL:
								case sql::DataType::DOUBLE:
								case sql::DataType::DECIMAL:
								case sql::DataType::NUMERIC: {
									line.push_back((double)res->getDouble(i));
								} break;
								case sql::DataType::DATE:
								case sql::DataType::TIME:
								case sql::DataType::TIMESTAMP:
								case sql::DataType::YEAR: {
									line.push_back(format_time(((sql2String(res->getString(i))).utf8().get_data()), false));
								} break;
								default: {
									line.push_back(sql2String(res->getString(i)));
								} break;
							}
						}
					}
					ret.push_back(line);
				}
			} break;
			case FUNC_EXEC_PREP: {
				prep_stmt.reset(con->prepareStatement(SQLquery));
				determine_datatype(prep_stmt, prep_val);
				afectedrows = prep_stmt->executeUpdate();
			} break;
		}

	}

	catch (sql::SQLException &e) {
		print_SQLException(e);
	} catch (runtime_error &e) {
		print_runtime_error(e);
	}

	return ret;
}

shared_ptr<sql::Connection> MySQL::connection(int what) {
	if (what == ACT_CLOSE) {
		if (con.get()) { // != NULL
			if (!con->isClosed()) {
				con->close();
			}
		}
	}

	if (what == ACT_DO) {
		if (con == NULL || (!con->isValid()) || (!con->reconnect())) {
			try {
				driver = get_mysql_driver_instance();
				con.reset(driver->connect(connection_properties));
			}

			catch (sql::SQLException &e) {
				print_SQLException(e);
			} catch (runtime_error &e) {
				print_runtime_error(e);
			}
		}
	}
	return con;
}

//-------------- Helpers
bool MySQL::check(int what) {
	shared_ptr<sql::Connection> con(connection(what));
	if (con != NULL) {
		if (!con->isClosed()) {
			return con->isValid();
		}
	}
	return false;
}

String MySQL::sql2String(const sql::SQLString &p_str) {
	const char *c = p_str.c_str();
	String str = String::utf8((char *)c);
	return str;
}

void MySQL::determine_datatype(std::shared_ptr<sql::PreparedStatement> prep_stmt, const Array &prep_val) {
	for (int i = 0; i < prep_val.size(); i++) {
		int d = i + 1;

		switch (prep_val[i].get_type()) {
			case Variant::Type::Nil: {
				prep_stmt->setNull(d, sql::DataType::SQLNULL);
			} break;
			case Variant::Type::BOOL: {
				prep_stmt->setBoolean(d, bool(prep_val[i]));
			} break;
			case Variant::Type::INT: {
				prep_stmt->setInt(d, int(prep_val[i]));
			} break;
			case Variant::Type::REAL: {
				prep_stmt->setDouble(d, float(prep_val[i]));
			} break;
			default: {
				String stri = String(prep_val[i]);
				sql::SQLString caracteres = stri.utf8().get_data();
				if (is_mysql_time(stri)) { // -- If the string has the mysql time type format, this gonna be handle as Date and Time types
					prep_stmt->setDateTime(d, caracteres);
				} else {
					prep_stmt->setString(d, caracteres);
				}
			}
		}
	}
}

Array MySQL::format_time(const String &str, bool return_string) {
	Array datando;
	string strss = str.utf8().get_data();
	char seps[] = ": -";
	char *token;
	char *next_token;
	token = strtok_s(&strss[0], seps, &next_token);
	while (token != NULL) {
		if (return_string) {
			datando.push_back(String(token)); //--As String
		} else {
			datando.push_back(atoi(token)); //--As Data
		}
		token = strtok_s(NULL, seps, &next_token);
	}
	return datando;
}

bool MySQL::is_mysql_time(const String &time) {
	string s_time = time.utf8().get_data(); ///Impo
	int len = time.length();
	if (s_time.find_first_not_of("0123456789:- ") == string::npos) {

		switch (len) {
			case 4: {
				if (s_time.find_first_not_of("0123456789") == string::npos) {
					return true;
				}
			} break;
			case 8: {
				if (time[2] == ':' && time[5] == ':') {
					Array arr_time = format_time(time, true);
					if (arr_time.size() == 3 && String(arr_time[2]).length() == 2) {
						return true;
					}
				}
			} break;
			case 10: {
				if (time[4] == '-' && time[7] == '-') {
					Array arr_time = format_time(time, true);
					if (arr_time.size() == 3 && String(arr_time[2]).length() == 2) {
						return true;
					}
				}
			} break;
			case 19: {
				if (time[4] == '-' && time[7] == '-' && time[13] == ':' && time[16] == ':' && time[10] == ' ') {
					Array arr_time = format_time(time, true);
					if (arr_time.size() == 6 && String(arr_time[2]).length() == 2) {
						return true;
					}
				}
			} break;
			default:
				break;
		}
	}
	return false;
}

void MySQL::print_SQLException(sql::SQLException &e) {
	//If (e.getErrorCode() == 1047) = No prepareted statement support at all.

	print_line("# EXCEPTION Caught ˇ");
	Variant file = __FILE__;
	Variant line = __LINE__;
	Variant func = __FUNCTION__;
	print_line("# ERR: SQLException in: " + String(file) + " in function: " + String(func) + "() on line " + String(line));
	print_line("# ERR: " + String(e.what()));
	Variant errCode = e.getErrorCode();
	print_line(" (MySQL error code: " + String(errCode) + ")");
	print_line("SQLState: " + sql2String(e.getSQLState()));
}

void MySQL::print_runtime_error(runtime_error &e) {
	cout << "ERROR: runtime_error in " << __FILE__;
	cout << " (" << __func__ << ") on line " << __LINE__ << endl;
	cout << "ERROR: " << e.what() << endl;
}

void MySQL::_bind_methods() {
	//--- Connection Managers
	ClassDB::bind_method(D_METHOD("connection_start"), &MySQL::connection_start);
	ClassDB::bind_method(D_METHOD("connection_check"), &MySQL::connection_check);
	ClassDB::bind_method(D_METHOD("connection_close"), &MySQL::connection_close);
	ClassDB::bind_method(D_METHOD("set_credentials", "Host", "User", "Password"), &MySQL::set_credentials);
	ClassDB::bind_method(D_METHOD("set_client_options", "Option", "Value"), &MySQL::set_client_options);
	ClassDB::bind_method(D_METHOD("get_client_options", "Option"), &MySQL::get_client_options);
	//--- Database
	ClassDB::bind_method(D_METHOD("set_database", "Database"), &MySQL::set_database);
	ClassDB::bind_method(D_METHOD("get_database"), &MySQL::get_database);
	//--- Prepared Query
	ClassDB::bind_method(D_METHOD("prep_execute", "SQL_execute", "values"), &MySQL::prep_execute);
	ClassDB::bind_method(D_METHOD("prep_fetch_dictionary", "SQL_query", "values", "string"), &MySQL::prep_fetch_dictionary);
	ClassDB::bind_method(D_METHOD("prep_fetch_array", "SQL_query", "values", "string"), &MySQL::prep_fetch_array);
	ClassDB::bind_method(D_METHOD("prep_get_columns_names", "SQL_query", "values"), &MySQL::prep_get_columns_names);
	ClassDB::bind_method(D_METHOD("prep_get_columns_types", "SQL_query", "values"), &MySQL::prep_get_columns_types);
	//---  Query
	ClassDB::bind_method(D_METHOD("query_execute", "SQL_execute"), &MySQL::query_execute);
	ClassDB::bind_method(D_METHOD("query_fetch_dictionary", "SQL_query", "string"), &MySQL::query_fetch_dictionary);
	ClassDB::bind_method(D_METHOD("query_fetch_array", "SQL_query", "string"), &MySQL::query_fetch_array);
	ClassDB::bind_method(D_METHOD("query_get_columns_names", "SQL_query"), &MySQL::query_get_columns_names);
	ClassDB::bind_method(D_METHOD("query_get_columns_types", "SQL_query"), &MySQL::query_get_columns_types);
}

MySQL::MySQL() {
	connection_properties["port"] = 3306;
	connection_properties["OPT_RECONNECT"] = true;
	afectedrows = 0;
}

MySQL::~MySQL() {
	connection_close();
}
