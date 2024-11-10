#ifndef INC_DB_INTERFACE_H
#define INC_DB_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <vector>
#include <optional>
#include <sqlite3.h>

using namespace std;
namespace second_take {

#define DB_CONNECTION_PARAM_FILE_NAME "fileName"

class Field {
    public:
        Field(string value);
        Field(string value, bool isNull);
        ~Field() = default;
        string getValue();
        void setValue(string value);
        void setIsNull(bool isNull);
        bool getIsNull();
    private:
        string value;
        bool isNull;
};

class Row {
    public:
        Row();
        ~Row() = default;
        optional<string> getFieldValue(string fieldName);
        bool getIsFieldValueNull(string fieldName);
        void setFieldValue(string fieldName, string value);
        void setFieldValue(string fieldName, string value, bool isNull);
    private:
        shared_ptr<map<string, Field>> fields;
};

class RecordSet {
    public:
        RecordSet();
        virtual ~RecordSet() = default;
        shared_ptr<Row> addNewRow();
        uint getRowCount();
        optional<string> getValue(string fieldName, uint rowIndex);
        bool getIsNull(string fieldName, uint rowIndex);
    private:
        shared_ptr<vector<shared_ptr<Row>>> rows;
};

class Database {
    public:
        virtual ~Database() = default;
        virtual bool open(map<string, string>& connectionParams) = 0;
        virtual void close() = 0;
        virtual shared_ptr<RecordSet> executeQuery(string sql) = 0;
        virtual bool isOpen();
    protected:
        bool connectionIsOpen = false;
};

class DatabaseSQLite : public Database {
    public:
        DatabaseSQLite() = default;
        ~DatabaseSQLite() = default;
        bool open(map<string, string>& connectionParams);
        void close();
        shared_ptr<RecordSet> executeQuery(string sql);
       private:
        sqlite3* dbConnection;
        void logSqLiteError(string errorMsgPrefix);
        void logErrorLackingParameter(string paramaterName);
        void logOpenSuccess();
        void logSqlExecuteError(char* errorMessage, string sql);
};

}

#endif // INC_DB_INTERFACE_H