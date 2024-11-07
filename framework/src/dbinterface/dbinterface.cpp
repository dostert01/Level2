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
        auto fileName = connectionParams.find(DB_CONNECTION_PARAM_FILE_NAME);
        if(fileName != connectionParams.end()) {
            int status = sqlite3_open(fileName->second.c_str(), &dbConnection);
            connectionIsOpen = (status == SQLITE_OK);
        } else {
          logErrorLackingParameter(DB_CONNECTION_PARAM_FILE_NAME);
        }
    }
    logOpenSuccess();
    return isOpen();
}

void DatabaseSQLite::close() {
    if(isOpen()) {
        if(sqlite3_close(dbConnection) == SQLITE_OK) {
            connectionIsOpen = false;
            LOGGER.info("DB closed successful!");
        } else {
            logSqLiteError("Closing DB failed.");
        }
    }
}

shared_ptr<RecordSet> DatabaseSQLite::executeQuery(string sql) {
    return nullptr;
}

void DatabaseSQLite::logOpenSuccess() {
    if (!isOpen()) {
        logSqLiteError("Opening DB failed.");
    } else {
        LOGGER.info("DB opened successful!");
    }
}

void DatabaseSQLite::logSqLiteError(string errorMsgPrefix) {
    LOGGER.error(errorMsgPrefix + "  sqlite3_errmsg reports: '" + sqlite3_errmsg(dbConnection) + "'");
}

void DatabaseSQLite::logErrorLackingParameter(string paramaterName) {
    LOGGER.error("Failed to open DB. connectionParams is lacking parameter '" + paramaterName + "'");
}


}
