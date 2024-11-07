#ifndef INC_DB_INTERFACE_H
#define INC_DB_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <sqlite3.h>

using namespace std;
namespace second_take {

#define DB_CONNECTION_PARAM_FILE_NAME "fileName"

class RecordSet {
    public:
        virtual ~RecordSet() = 0;
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
};

}

#endif // INC_DB_INTERFACE_H