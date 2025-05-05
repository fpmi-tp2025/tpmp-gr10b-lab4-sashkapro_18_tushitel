#include "db_utils.hpp"
#include <iostream>
#include <string>
#include <limits>

bool login(sqlite3* db) {
    std::string username, password;
    int attempts = 3;

    while (attempts > 0) {
        std::cout << "\n=== Вход в систему ===\n";
        std::cout << "Осталось попыток: " << attempts << "\n";
        std::cout << "Логин: ";
        std::cin >> username;
        std::cout << "Пароль: ";
        std::cin >> password;

        if (authenticateUser(db, username, password)) {
            std::cout << "Успешный вход!\n";
            return true;
        } else {
            std::cout << "Ошибка входа! Неверный логин или пароль.\n";
            attempts--;
        }
    }

    std::cout << "Превышено количество попыток входа. Программа завершается.\n";
    return false;
}

void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
