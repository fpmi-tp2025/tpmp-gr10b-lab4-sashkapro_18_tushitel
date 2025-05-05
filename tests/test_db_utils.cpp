#include "db_utils.hpp"
#include <gtest/gtest.h>
#include <filesystem>

class DBUtilsTest : public ::testing::Test {
protected:
    sqlite3* db;
    std::string testDbName = "test_autoshops.db";

    void SetUp() override {
        // Удаляем тестовую БД если она существует
        if (std::filesystem::exists(testDbName)) {
            std::filesystem::remove(testDbName);
        }
        // Открываем новую тестовую БД
        ASSERT_TRUE(openDatabase(&db, testDbName));
        ASSERT_TRUE(initializeDatabase(db));
    }

    void TearDown() override {
        closeDatabase(db);
        // Удаляем тестовую БД после тестов
        if (std::filesystem::exists(testDbName)) {
            std::filesystem::remove(testDbName);
        }
    }
};

// Тест 1: Проверка аутентификации
TEST_F(DBUtilsTest, AuthenticationTest) {
    // Проверяем, что admin может войти с правильным паролем
    EXPECT_TRUE(authenticateUser(db, "admin", "admin123"));
    
    // Проверяем, что с неправильным паролем войти нельзя
    EXPECT_FALSE(authenticateUser(db, "admin", "wrong_password"));
}

// Тест 2: Проверка добавления ремонта
TEST_F(DBUtilsTest, AddRepairTest) {
    Repair repair;
    repair.client_name = "Иванов";
    repair.car_model = "Toyota";
    repair.description = "Замена масла";
    repair.status = "В ожидании";
    repair.cost = 1000.0;
    repair.start_date = "2024-03-20";
    
    // Добавляем ремонт
    EXPECT_TRUE(addRepair(db, repair));
    
    // Проверяем, что ремонт добавился
    auto repairs = getAllRepairs(db);
    EXPECT_EQ(repairs.size(), 1);
}

// Тест 3: Проверка удаления ремонта
TEST_F(DBUtilsTest, DeleteRepairTest) {
    // Сначала добавляем ремонт
    Repair repair;
    repair.client_name = "Иванов";
    repair.car_model = "Toyota";
    repair.description = "Замена масла";
    repair.status = "В ожидании";
    repair.cost = 1000.0;
    repair.start_date = "2024-03-20";
    
    ASSERT_TRUE(addRepair(db, repair));
    
    // Получаем ID добавленного ремонта
    auto repairs = getAllRepairs(db);
    ASSERT_EQ(repairs.size(), 1);
    int repair_id = repairs[0].id;
    
    // Удаляем ремонт
    EXPECT_TRUE(deleteRepair(db, repair_id));
    
    // Проверяем, что ремонт удалился
    repairs = getAllRepairs(db);
    EXPECT_EQ(repairs.size(), 0);
} 