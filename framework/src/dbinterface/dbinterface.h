#ifndef INC_DB_INTERFACE_H
#define INC_DB_INTERFACE_H

#include <string>
#include <memory>
#include <map>
#include <sqlite3.h>

using namespace std;
namespace second_take {

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

#define SQLITE_DB_CONNECTION_PARAM_FILE_NAME "fileName"

class DatabaseSQLite : public Database {
    public:
        DatabaseSQLite() = default;
        ~DatabaseSQLite() = default;
        bool open(map<string, string>& connectionParams);
        void close();
        shared_ptr<RecordSet> executeQuery(string sql);
    private:
        sqlite3* dbConnection;
};

}

#endif // INC_DB_INTERFACE_H