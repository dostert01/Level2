#pragma once

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <optional>
#include <sqlite3.h>

namespace level2 {

#define DB_CONNECTION_PARAM_FILE_NAME "fileName"

class Field {
    public:
        Field(std::string value);
        Field(std::string value, bool isNull);
        ~Field() = default;
        std::string getValue();
        void setValue(std::string value);
        void setIsNull(bool isNull);
        bool getIsNull();
    private:
        std::string value;
        bool isNull;
};

class Row {
    public:
        Row();
        ~Row() = default;
        std::optional<std::string> getFieldValue(std::string fieldName);
        bool getIsFieldValueNull(std::string fieldName);
        void setFieldValue(std::string fieldName, std::string value);
        void setFieldValue(std::string fieldName, std::string value, bool isNull);
    private:
        std::shared_ptr<std::map<std::string, Field>> fields;
};

class RecordSet {
    public:
        RecordSet();
        virtual ~RecordSet() = default;
        std::shared_ptr<Row> addNewRow();
        uint getRowCount();
        std::optional<std::string> getValue(std::string fieldName, uint rowIndex);
        bool getIsNull(std::string fieldName, uint rowIndex);
    private:
        std::shared_ptr<std::vector<std::shared_ptr<Row>>> rows;
};

class Database {
    public:
        virtual ~Database() = default;
        virtual bool open(std::map<std::string, std::string>& connectionParams) = 0;
        virtual void close() = 0;
        virtual std::shared_ptr<RecordSet> executeQuery(std::string sql) = 0;
        virtual bool isOpen();
    protected:
        bool connectionIsOpen = false;
};

class DatabaseSQLite : public Database {
    public:
        DatabaseSQLite() = default;
        ~DatabaseSQLite() = default;
        bool open(std::map<std::string, std::string>& connectionParams);
        void close();
        std::shared_ptr<RecordSet> executeQuery(std::string sql);
       private:
        sqlite3* dbConnection;
        void logSqLiteError(std::string errorMsgPrefix);
        void logErrorLackingParameter(std::string paramaterName);
        void logOpenSuccess();
        void logSqlExecuteError(char* errorMessage, std::string sql);
};

}
