#include "dbinterface.h"
#include "../logger/logger.h"

namespace event_forge {
    
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

static int rowReaderCallback(void *instanceOfRecordSet, int argc, char **argv, char **azColName) {
    int i;
    if(instanceOfRecordSet != NULL) {
        RecordSet* resultSet = (RecordSet*)instanceOfRecordSet;
        shared_ptr<Row> newRow = resultSet->addNewRow();
        for(i = 0; i<argc; i++) {
            newRow->setFieldValue(azColName[i], argv[i] ? argv[i] : "NULL", argv[i] == NULL);
        }
    }
    return 0;
}

shared_ptr<RecordSet> DatabaseSQLite::executeQuery(string sql) {
    char *errorMessage = NULL;
    auto result = make_shared<RecordSet>();
    if(sqlite3_exec(dbConnection, sql.c_str(), rowReaderCallback, result.get(), &errorMessage) == SQLITE_OK) {
        LOGGER.info("executed SQL '" + sql + "'");
    } else {
        logSqlExecuteError(errorMessage, sql);
    }
    return result;
}

void DatabaseSQLite::logSqlExecuteError(char *errorMessage, string sql) {
  if (errorMessage != NULL) {
    LOGGER.error("failed executing SQL '" + sql + "' : " + errorMessage);
    sqlite3_free(errorMessage);
  } else {
    LOGGER.error("failed executing SQL '" + sql + "'");
  }
}

void DatabaseSQLite::logOpenSuccess() {
    if (!isOpen()) {
        logSqLiteError("Opening DB failed.");
    } else {
        LOGGER.info("DB opened successful!");
    }
}

void DatabaseSQLite::logSqLiteError(string errorMsgPrefix) {
    LOGGER.error(errorMsgPrefix + " sqlite3_errmsg reports '" + sqlite3_errmsg(dbConnection) + "'");
}

void DatabaseSQLite::logErrorLackingParameter(string paramaterName) {
    LOGGER.error("Failed to open DB. connectionParams is lacking parameter '" + paramaterName + "'");
}

/*
    RecordSet
*/
RecordSet::RecordSet() {
    rows = make_shared<vector<shared_ptr<Row>>>();
}

shared_ptr<Row> RecordSet::addNewRow() {
    auto newRow = make_shared<Row>();
    rows->push_back(newRow);
    return newRow;
}

uint RecordSet::getRowCount() {
    return rows->size();
}

optional<string> RecordSet::getValue(string fieldName, uint rowIndex) {
    optional<string> fieldValue = nullopt;
    if(rowIndex < getRowCount()) {
        fieldValue = rows->at(rowIndex)->getFieldValue(fieldName);
    }
    if(!fieldValue.has_value()) {
        LOGGER.error("No value found for column " + fieldName + " in row " + to_string(rowIndex));
    }
    return fieldValue;
}

bool RecordSet::getIsNull(string fieldName, uint rowIndex) {
    bool fieldValue = true;
    if(rowIndex < getRowCount()) {
        fieldValue = rows->at(rowIndex)->getIsFieldValueNull(fieldName);
    } else {
        LOGGER.error("Row index '" + to_string(rowIndex) +
            "' is out of bounds while reading property of field '" + fieldName + "'");
    }
    return fieldValue;
}

/*
    Row
*/
Row::Row() {
    fields = make_shared<map<string, Field>>();
}

optional<string> Row::getFieldValue(string fieldName) {
    optional<string> returnValue = nullopt;
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        returnValue = field->second.getValue();
    }
    if(!returnValue.has_value()) {
        LOGGER.error("Value for fieldName '" + fieldName + "' not found!");
    }
    return returnValue;
}

bool Row::getIsFieldValueNull(string fieldName) {
    bool returnValue = true;
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        returnValue = field->second.getIsNull();
    } else {
        LOGGER.error("Field with fieldName '" + fieldName + "' not found in Row!");
    }
    return returnValue;
}

void Row::setFieldValue(string fieldName, string value, bool isNull) {
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        field->second.setValue(value);
        field->second.setIsNull(isNull);
    } else {
        fields->emplace(fieldName, Field(value, isNull));
    }
}

void Row::setFieldValue(string fieldName, string value) {
    setFieldValue(fieldName, value, false);
}

/*
    Field
*/
Field::Field(string value) : Field(value, false) {

}

Field::Field(string value, bool isNull) {
    this->setValue(value);
    this->setIsNull(isNull);
}

void Field::setValue(string value) {
    this->value = value;
}

void Field::setIsNull(bool isNull) {
    this->isNull = isNull;
}

bool Field::getIsNull() {
    return isNull;
}

string Field::getValue() {
    return value;
}

}
