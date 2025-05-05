#include "image_utils.hpp"
#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

class ImageUtilsTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Создаем тестовую директорию для изображений
        std::filesystem::create_directories("test_images");
    }

    void TearDown() override {
        // Удаляем тестовую директорию
        std::filesystem::remove_all("test_images");
    }
};

// Тест создания директории для изображений
TEST_F(ImageUtilsTest, CreateImageDirectoryTest) {
    EXPECT_TRUE(createImageDirectory());
    EXPECT_TRUE(std::filesystem::exists("images"));
}

// Тест сохранения изображения
TEST_F(ImageUtilsTest, SaveImageTest) {
    // Создаем тестовый файл
    std::string testImagePath = "test_images/test.jpg";
    std::ofstream testFile(testImagePath);
    testFile << "test image content";
    testFile.close();

    // Сохраняем изображение
    std::string savedPath = saveImage(testImagePath);
    EXPECT_FALSE(savedPath.empty());
    EXPECT_TRUE(std::filesystem::exists(savedPath));

    // Проверяем, что файл был скопирован
    std::ifstream savedFile(savedPath);
    std::string content;
    std::getline(savedFile, content);
    EXPECT_EQ(content, "test image content");
}

// Тест ввода изображений
TEST_F(ImageUtilsTest, InputImagesTest) {
    // Создаем тестовые файлы
    std::vector<std::string> testFiles = {
        "test_images/test1.jpg",
        "test_images/test2.jpg"
    };

    for (const auto& file : testFiles) {
        std::ofstream testFile(file);
        testFile << "test content";
        testFile.close();
    }

    // Симулируем ввод путей к изображениям
    std::istringstream input("test_images/test1.jpg\ntest_images/test2.jpg\ndone\n");
    std::streambuf* orig = std::cin.rdbuf(input.rdbuf());

    auto images = inputImages();
    std::cin.rdbuf(orig);

    EXPECT_EQ(images.size(), 2);
    for (const auto& path : images) {
        EXPECT_TRUE(std::filesystem::exists(path));
    }
} 