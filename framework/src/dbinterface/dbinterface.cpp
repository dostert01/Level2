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
        auto fileName = connectionParams.find(SQLITE_DB_CONNECTION_PARAM_FILE_NAME);
        if(fileName != connectionParams.end()) {
            int status = sqlite3_open(fileName->second.c_str(), &dbConnection);
            connectionIsOpen = (status == SQLITE_OK);
        } else {
            string s = SQLITE_DB_CONNECTION_PARAM_FILE_NAME;
            LOGGER.error("Failed to open DB. connectionParams is lacking parameter '" + s + "'");
        }
    }
    if(!isOpen()) {
        string errorMsgPrefix = "Opening DB failed. sqlite3_errmsg reports: ";
        LOGGER.error(errorMsgPrefix + sqlite3_errmsg(dbConnection));
    } else {
        LOGGER.info("DB opened successful!");
    }
    return isOpen();
}

void DatabaseSQLite::close() {
    if(isOpen()) {
        if(sqlite3_close(dbConnection) == SQLITE_OK) {
            connectionIsOpen = false;
            LOGGER.info("DB closed successful!");
        } else {
            string errorMsgPrefix = "Closing DB failed. sqlite3_errmsg reports: ";
            LOGGER.error(errorMsgPrefix + sqlite3_errmsg(dbConnection));
        }
    }
}

shared_ptr<RecordSet> DatabaseSQLite::executeQuery(string sql) {
    return nullptr;
}


}
