#ifndef AUTOSHOP_DB_UTILS_HPP
#define AUTOSHOP_DB_UTILS_HPP

#include <sqlite3.h>
#include <string>
#include <vector>
#include <memory>

struct User {
    int id;
    std::string username;
    std::string password;
    std::string role;
};

struct Repair {
    int id;
    std::string client_name;
    std::string car_model;
    std::string description;
    std::string status;
    double cost;
    std::string start_date;
    std::string end_date;
    std::vector<std::string> images;
};

bool openDatabase(sqlite3** db, const std::string& dbName);
void closeDatabase(sqlite3* db);
bool authenticateUser(sqlite3* db, const std::string& username, const std::string& password);
bool initializeDatabase(sqlite3* db);
std::vector<Repair> getAllRepairs(sqlite3* db);
bool addRepair(sqlite3* db, const Repair& repair);
bool updateRepair(sqlite3* db, const Repair& repair);
bool deleteRepair(sqlite3* db, int repairId);
std::vector<Repair> searchRepairs(sqlite3* db, const std::string& query);

#endif
