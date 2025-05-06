#pragma once

#include "db_utils.hpp"
#include <iostream>
#include <string>

void showMenu();
void displayRepair(const Repair& repair);
void displayAllRepairs(sqlite3* db);
Repair inputRepairData();
void handleUserAction(sqlite3* db, int choice); 