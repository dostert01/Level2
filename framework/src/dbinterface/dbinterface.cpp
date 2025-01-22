#include "dbinterface.h"
#include "logger.h"

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
bool DatabaseSQLite::open(std::map<std::string, std::string>& connectionParams) {
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
        std::shared_ptr<Row> newRow = resultSet->addNewRow();
        for(i = 0; i<argc; i++) {
            newRow->setFieldValue(azColName[i], argv[i] ? argv[i] : "NULL", argv[i] == NULL);
        }
    }
    return 0;
}

std::shared_ptr<RecordSet> DatabaseSQLite::executeQuery(std::string sql) {
    char *errorMessage = NULL;
    auto result = std::make_shared<RecordSet>();
    if(sqlite3_exec(dbConnection, sql.c_str(), rowReaderCallback, result.get(), &errorMessage) == SQLITE_OK) {
        LOGGER.info("executed SQL '" + sql + "'");
    } else {
        logSqlExecuteError(errorMessage, sql);
    }
    return result;
}

void DatabaseSQLite::logSqlExecuteError(char *errorMessage, std::string sql) {
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

void DatabaseSQLite::logSqLiteError(std::string errorMsgPrefix) {
    LOGGER.error(errorMsgPrefix + " sqlite3_errmsg reports '" + sqlite3_errmsg(dbConnection) + "'");
}

void DatabaseSQLite::logErrorLackingParameter(std::string paramaterName) {
    LOGGER.error("Failed to open DB. connectionParams is lacking parameter '" + paramaterName + "'");
}

/*
    RecordSet
*/
RecordSet::RecordSet() {
    rows = std::make_shared<std::vector<std::shared_ptr<Row>>>();
}

std::shared_ptr<Row> RecordSet::addNewRow() {
    auto newRow = std::make_shared<Row>();
    rows->push_back(newRow);
    return newRow;
}

uint RecordSet::getRowCount() {
    return rows->size();
}

std::optional<std::string> RecordSet::getValue(std::string fieldName, uint rowIndex) {
    std::optional<std::string> fieldValue = std::nullopt;
    if(rowIndex < getRowCount()) {
        fieldValue = rows->at(rowIndex)->getFieldValue(fieldName);
    }
    if(!fieldValue.has_value()) {
        LOGGER.error("No value found for column " + fieldName + " in row " + std::to_string(rowIndex));
    }
    return fieldValue;
}

bool RecordSet::getIsNull(std::string fieldName, uint rowIndex) {
    bool fieldValue = true;
    if(rowIndex < getRowCount()) {
        fieldValue = rows->at(rowIndex)->getIsFieldValueNull(fieldName);
    } else {
        LOGGER.error("Row index '" + std::to_string(rowIndex) +
            "' is out of bounds while reading property of field '" + fieldName + "'");
    }
    return fieldValue;
}

/*
    Row
*/
Row::Row() {
    fields = std::make_shared<std::map<std::string, Field>>();
}

std::optional<std::string> Row::getFieldValue(std::string fieldName) {
    std::optional<std::string> returnValue = std::nullopt;
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        returnValue = field->second.getValue();
    }
    if(!returnValue.has_value()) {
        LOGGER.error("Value for fieldName '" + fieldName + "' not found!");
    }
    return returnValue;
}

bool Row::getIsFieldValueNull(std::string fieldName) {
    bool returnValue = true;
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        returnValue = field->second.getIsNull();
    } else {
        LOGGER.error("Field with fieldName '" + fieldName + "' not found in Row!");
    }
    return returnValue;
}

void Row::setFieldValue(std::string fieldName, std::string value, bool isNull) {
    auto field = fields->find(fieldName);
    if(field != fields->end()) {
        field->second.setValue(value);
        field->second.setIsNull(isNull);
    } else {
        fields->emplace(fieldName, Field(value, isNull));
    }
}

void Row::setFieldValue(std::string fieldName, std::string value) {
    setFieldValue(fieldName, value, false);
}

/*
    Field
*/
Field::Field(std::string value) : Field(value, false) {

}

Field::Field(std::string value, bool isNull) {
    this->setValue(value);
    this->setIsNull(isNull);
}

void Field::setValue(std::string value) {
    this->value = value;
}

void Field::setIsNull(bool isNull) {
    this->isNull = isNull;
}

bool Field::getIsNull() {
    return isNull;
}

std::string Field::getValue() {
    return value;
}

}
