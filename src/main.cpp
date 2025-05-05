#include "db_utils.hpp"
#include "main_utils.hpp"
#include <iostream>

extern bool login(sqlite3* db);

int main() {
    sqlite3* db;
    if (!openDatabase(&db, "autoshops.db")) {
        std::cerr << "Ошибка при открытии БД!\n";
        return 1;
    }

    if (!initializeDatabase(db)) {
        std::cerr << "Ошибка при инициализации БД!\n";
        closeDatabase(db);
        return 1;
    }

    if (!login(db)) {
        closeDatabase(db);
        return 1;
    }

    int choice;
    do {
        showMenu();
        std::cin >> choice;
        if (choice != 0) {
            handleUserAction(db, choice);
        }
    } while (choice != 0);

    closeDatabase(db);
    return 0;
} 