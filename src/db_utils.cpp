#include "db_utils.hpp"
#include <iostream>
#include <sstream>
#include <filesystem>

bool openDatabase(sqlite3** db, const std::string& dbName) {
    return sqlite3_open(dbName.c_str(), db) == SQLITE_OK;
}

void closeDatabase(sqlite3* db) {
    sqlite3_close(db);
}

bool initializeDatabase(sqlite3* db) {
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            role TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS repairs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            client_name TEXT NOT NULL,
            car_model TEXT NOT NULL,
            description TEXT,
            status TEXT NOT NULL,
            cost REAL,
            start_date TEXT NOT NULL,
            end_date TEXT,
            created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        );

        CREATE TABLE IF NOT EXISTS repair_images (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            repair_id INTEGER,
            image_path TEXT NOT NULL,
            FOREIGN KEY (repair_id) REFERENCES repairs(id)
        );

        INSERT OR IGNORE INTO users (username, password, role)
        VALUES ('admin', 'admin123', 'admin');
    )";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Ошибка при инициализации базы данных: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool authenticateUser(sqlite3* db, const std::string& username, const std::string& password) {
    std::string query = "SELECT COUNT(*) FROM users WHERE username=? AND password=?";
    sqlite3_stmt* stmt;
    bool result = false;

    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_int(stmt, 0) > 0;
        }
    }
    sqlite3_finalize(stmt);
    return result;
}

std::vector<Repair> getAllRepairs(sqlite3* db) {
    std::vector<Repair> repairs;
    const char* sql = "SELECT * FROM repairs ORDER BY created_at DESC";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Repair repair;
            repair.id = sqlite3_column_int(stmt, 0);
            repair.client_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            repair.car_model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            repair.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            repair.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            repair.cost = sqlite3_column_double(stmt, 5);
            repair.start_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            repair.end_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            
            const char* img_sql = "SELECT image_path FROM repair_images WHERE repair_id = ?";
            sqlite3_stmt* img_stmt;
            if (sqlite3_prepare_v2(db, img_sql, -1, &img_stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(img_stmt, 1, repair.id);
                while (sqlite3_step(img_stmt) == SQLITE_ROW) {
                    repair.images.push_back(reinterpret_cast<const char*>(sqlite3_column_text(img_stmt, 0)));
                }
            }
            sqlite3_finalize(img_stmt);
            
            repairs.push_back(repair);
        }
    }
    sqlite3_finalize(stmt);
    return repairs;
}

bool addRepair(sqlite3* db, const Repair& repair) {
    const char* sql = "INSERT INTO repairs (client_name, car_model, description, status, cost, start_date, end_date) "
                     "VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, repair.client_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, repair.car_model.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, repair.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, repair.status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 5, repair.cost);
        sqlite3_bind_text(stmt, 6, repair.start_date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, repair.end_date.c_str(), -1, SQLITE_STATIC);

        success = sqlite3_step(stmt) == SQLITE_DONE;
        
        if (success && !repair.images.empty()) {
            int repair_id = sqlite3_last_insert_rowid(db);
            const char* img_sql = "INSERT INTO repair_images (repair_id, image_path) VALUES (?, ?)";
            sqlite3_stmt* img_stmt;
            
            if (sqlite3_prepare_v2(db, img_sql, -1, &img_stmt, nullptr) == SQLITE_OK) {
                for (const auto& image : repair.images) {
                    sqlite3_bind_int(img_stmt, 1, repair_id);
                    sqlite3_bind_text(img_stmt, 2, image.c_str(), -1, SQLITE_STATIC);
                    sqlite3_step(img_stmt);
                    sqlite3_reset(img_stmt);
                }
            }
            sqlite3_finalize(img_stmt);
        }
    }
    sqlite3_finalize(stmt);
    return success;
}

bool updateRepair(sqlite3* db, const Repair& repair) {
    const char* sql = "UPDATE repairs SET client_name=?, car_model=?, description=?, status=?, "
                     "cost=?, start_date=?, end_date=? WHERE id=?";
    sqlite3_stmt* stmt;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, repair.client_name.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, repair.car_model.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, repair.description.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, repair.status.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 5, repair.cost);
        sqlite3_bind_text(stmt, 6, repair.start_date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 7, repair.end_date.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 8, repair.id);

        success = sqlite3_step(stmt) == SQLITE_DONE;
    }
    sqlite3_finalize(stmt);
    return success;
}

bool deleteRepair(sqlite3* db, int repairId) {
    const char* sql = "DELETE FROM repairs WHERE id=?";
    sqlite3_stmt* stmt;
    bool success = false;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, repairId);
        success = sqlite3_step(stmt) == SQLITE_DONE;
    }
    sqlite3_finalize(stmt);
    return success;
}

std::vector<Repair> searchRepairs(sqlite3* db, const std::string& query) {
    std::vector<Repair> repairs;
    std::string sql = "SELECT * FROM repairs WHERE client_name LIKE ? OR car_model LIKE ? OR description LIKE ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        std::string search_pattern = "%" + query + "%";
        sqlite3_bind_text(stmt, 1, search_pattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, search_pattern.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, search_pattern.c_str(), -1, SQLITE_STATIC);

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Repair repair;
            repair.id = sqlite3_column_int(stmt, 0);
            repair.client_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            repair.car_model = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            repair.description = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            repair.status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            repair.cost = sqlite3_column_double(stmt, 5);
            repair.start_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 6));
            repair.end_date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 7));
            
            // Загружаем изображения для ремонта
            const char* img_sql = "SELECT image_path FROM repair_images WHERE repair_id = ?";
            sqlite3_stmt* img_stmt;
            if (sqlite3_prepare_v2(db, img_sql, -1, &img_stmt, nullptr) == SQLITE_OK) {
                sqlite3_bind_int(img_stmt, 1, repair.id);
                while (sqlite3_step(img_stmt) == SQLITE_ROW) {
                    repair.images.push_back(reinterpret_cast<const char*>(sqlite3_column_text(img_stmt, 0)));
                }
            }
            sqlite3_finalize(img_stmt);
            
            repairs.push_back(repair);
        }
    }
    sqlite3_finalize(stmt);
    return repairs;
}
