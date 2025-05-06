#include "db_utils.hpp"
#include <gtest/gtest.h>
#include <filesystem>

class DBUtilsTest : public ::testing::Test {
protected:
    sqlite3* db;
    std::string testDbName = "test_autoshops.db";

    void SetUp() override {
        if (std::filesystem::exists(testDbName)) {
            std::filesystem::remove(testDbName);
        }
        ASSERT_TRUE(openDatabase(&db, testDbName));
        ASSERT_TRUE(initializeDatabase(db));
    }

    void TearDown() override {
        closeDatabase(db);
        if (std::filesystem::exists(testDbName)) {
            std::filesystem::remove(testDbName);
        }
    }
};

TEST_F(DBUtilsTest, AuthenticationTest) {
    EXPECT_TRUE(authenticateUser(db, "admin", "admin123"));
    
    EXPECT_FALSE(authenticateUser(db, "admin", "wrong_password"));
}

TEST_F(DBUtilsTest, AddRepairTest) {
    Repair repair;
    repair.client_name = "Иванов";
    repair.car_model = "Toyota";
    repair.description = "Замена масла";
    repair.status = "В ожидании";
    repair.cost = 1000.0;
    repair.start_date = "2024-03-20";
    
    EXPECT_TRUE(addRepair(db, repair));
    
    auto repairs = getAllRepairs(db);
    EXPECT_EQ(repairs.size(), 1);
}

TEST_F(DBUtilsTest, DeleteRepairTest) {
    Repair repair;
    repair.client_name = "Иванов";
    repair.car_model = "Toyota";
    repair.description = "Замена масла";
    repair.status = "В ожидании";
    repair.cost = 1000.0;
    repair.start_date = "2024-03-20";
    
    ASSERT_TRUE(addRepair(db, repair));
    
    auto repairs = getAllRepairs(db);
    ASSERT_EQ(repairs.size(), 1);
    int repair_id = repairs[0].id;
    
    EXPECT_TRUE(deleteRepair(db, repair_id));
    
    repairs = getAllRepairs(db);
    EXPECT_EQ(repairs.size(), 0);
} 