#include "db_utils.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>

namespace fs = std::filesystem;

bool createImageDirectory() {
    try {
        fs::create_directories("images");
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при создании директории для изображений: " << e.what() << std::endl;
        return false;
    }
}

std::string saveImage(const std::string& sourcePath) {
    if (!fs::exists(sourcePath)) {
        std::cerr << "Файл не найден: " << sourcePath << std::endl;
        return "";
    }

    try {
        auto timestamp = std::chrono::system_clock::now().time_since_epoch().count();
        std::string filename = "images/repair_" + std::to_string(timestamp) + 
                             fs::path(sourcePath).extension().string();
        
        fs::copy_file(sourcePath, filename, fs::copy_options::overwrite_existing);
        return filename;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка при сохранении изображения: " << e.what() << std::endl;
        return "";
    }
}

std::vector<std::string> inputImages() {
    std::vector<std::string> imagePaths;
    std::string path;
    
    std::cout << "\nДобавление изображений (введите 'done' для завершения):\n";
    while (true) {
        std::cout << "Введите путь к изображению (или 'done'): ";
        std::getline(std::cin, path);
        
        if (path == "done") break;
        
        std::string savedPath = saveImage(path);
        if (!savedPath.empty()) {
            imagePaths.push_back(savedPath);
            std::cout << "Изображение успешно добавлено\n";
        }
    }
    
    return imagePaths;
}

void displayImage(const std::string& imagePath) {
    if (!fs::exists(imagePath)) {
        std::cout << "Изображение не найдено: " << imagePath << std::endl;
        return;
    }
    
    std::cout << "Изображение доступно по пути: " << imagePath << std::endl;
}