/* register_types.cpp */

#include "register_types.h"
#include "core/class_db.h"
#include "mysql.h"

void register_mysqlcppconn_types() {
	ClassDB::register_class<MySQL>();
}

void unregister_mysqlcppconn_types() {
}

