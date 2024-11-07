#include "dbinterface.h"
#include "../logger/logger.h"

namespace second_take {
    
/*
    Database
*/
bool Database::isOpen() {
    return connectionIsOpen;
}

/*
    DatabaseSQLite
*/
bool DatabaseSQLite::open(map<string, string>& connectionParams) {
    if(!isOpen()) {
        string fileName = connectionParams[SQLITE_DB_CONNECTION_PARAM_FILE_NAME];
        int status = sqlite3_open(fileName.c_str(), &dbConnection);
        connectionIsOpen = (status == SQLITE_OK);
    }
    if(!isOpen()) {
        LOGGER.error(sqlite3_errmsg(dbConnection));
    }
    return isOpen();
}

void DatabaseSQLite::close() {
    if(isOpen()) {
        if(sqlite3_close(dbConnection) == SQLITE_OK) {
            connectionIsOpen = false;
        }
    }
}

shared_ptr<RecordSet> DatabaseSQLite::executeQuery(string sql) {
    return nullptr;
}


}
