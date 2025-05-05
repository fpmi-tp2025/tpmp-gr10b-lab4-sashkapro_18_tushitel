#ifndef AUTOSHOP_IMAGE_UTILS_HPP
#define AUTOSHOP_IMAGE_UTILS_HPP

#include <string>
#include <vector>

bool createImageDirectory();
std::string saveImage(const std::string& sourcePath);
std::vector<std::string> inputImages();
void displayImage(const std::string& imagePath);

#endif