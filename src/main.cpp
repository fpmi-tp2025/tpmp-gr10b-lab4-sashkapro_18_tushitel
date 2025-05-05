#include "db_utils.hpp"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <sstream>
#include <algorithm>

extern bool login(sqlite3* db);

void showMenu() {
    std::cout << "\n=== Автосервис - Система управления ремонтами ===\n";
    std::cout << "1. Показать все ремонты\n";
    std::cout << "2. Добавить новый ремонт\n";
    std::cout << "3. Поиск ремонтов\n";
    std::cout << "4. Обновить статус ремонта\n";
    std::cout << "5. Удалить ремонт\n";
    std::cout << "0. Выход\n";
    std::cout << "Выберите действие: ";
}

void displayRepair(const Repair& repair) {
    std::cout << "\n=== Информация о ремонте ===\n";
    std::cout << "ID: " << repair.id << "\n";
    std::cout << "Клиент: " << repair.client_name << "\n";
    std::cout << "Модель автомобиля: " << repair.car_model << "\n";
    std::cout << "Описание: " << repair.description << "\n";
    std::cout << "Статус: " << repair.status << "\n";
    std::cout << "Стоимость: " << std::fixed << std::setprecision(2) << repair.cost << " руб.\n";
    std::cout << "Дата начала: " << repair.start_date << "\n";
    if (!repair.end_date.empty()) {
        std::cout << "Дата окончания: " << repair.end_date << "\n";
    }
    if (!repair.images.empty()) {
        std::cout << "Изображения:\n";
        for (const auto& image : repair.images) {
            std::cout << "- " << image << "\n";
        }
    }
    std::cout << "===========================\n";
}

void displayAllRepairs(sqlite3* db) {
    auto repairs = getAllRepairs(db);
    if (repairs.empty()) {
        std::cout << "Нет доступных ремонтов.\n";
        return;
    }
    
    for (const auto& repair : repairs) {
        displayRepair(repair);
    }
}

Repair inputRepairData() {
    Repair repair;
    std::cout << "\nВведите данные о ремонте:\n";
    std::cout << "Имя клиента: ";
    std::cin.ignore();
    std::getline(std::cin, repair.client_name);
    
    std::cout << "Модель автомобиля: ";
    std::getline(std::cin, repair.car_model);
    
    std::cout << "Описание работ: ";
    std::getline(std::cin, repair.description);
    
    std::cout << "Стоимость: ";
    std::cin >> repair.cost;
    
    repair.status = "В ожидании";
    
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    repair.start_date = oss.str();
    
    return repair;
}

void handleUserAction(sqlite3* db, int choice) {
    switch (choice) {
        case 1: {
            displayAllRepairs(db);
            break;
        }
        case 2: {
            Repair repair = inputRepairData();
            if (addRepair(db, repair)) {
                std::cout << "Ремонт успешно добавлен!\n";
            } else {
                std::cout << "Ошибка при добавлении ремонта.\n";
            }
            break;
        }
        case 3: {
            std::string query;
            std::cout << "Введите поисковый запрос: ";
            std::cin.ignore();
            std::getline(std::cin, query);
            
            auto repairs = searchRepairs(db, query);
            if (repairs.empty()) {
                std::cout << "Ремонты не найдены.\n";
            } else {
                for (const auto& repair : repairs) {
                    displayRepair(repair);
                }
            }
            break;
        }
        case 4: {
            int repairId;
            std::cout << "Введите ID ремонта: ";
            std::cin >> repairId;
            
            auto repairs = getAllRepairs(db);
            auto it = std::find_if(repairs.begin(), repairs.end(),
                                 [repairId](const Repair& r) { return r.id == repairId; });
            
            if (it != repairs.end()) {
                std::cout << "Текущий статус: " << it->status << "\n";
                std::cout << "Новый статус (В работе/Завершен/Отменен): ";
                std::string newStatus;
                std::cin.ignore();
                std::getline(std::cin, newStatus);
                
                it->status = newStatus;
                if (updateRepair(db, *it)) {
                    std::cout << "Статус успешно обновлен!\n";
                } else {
                    std::cout << "Ошибка при обновлении статуса.\n";
                }
            } else {
                std::cout << "Ремонт не найден.\n";
            }
            break;
        }
        case 5: {
            int repairId;
            std::cout << "Введите ID ремонта для удаления: ";
            std::cin >> repairId;
            
            if (deleteRepair(db, repairId)) {
                std::cout << "Ремонт успешно удален!\n";
            } else {
                std::cout << "Ошибка при удалении ремонта.\n";
            }
            break;
        }
        default:
            std::cout << "Неверный выбор.\n";
    }
}

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
