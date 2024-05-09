#pragma once
// #include "stb_image.h" // This needs to be on top because of shitty include problems, stbi_image is included in Model.h!!!!
#include <string>
#include <vector>

class Utils
{
public:
    static unsigned int loadTexture(char const *path);

    static unsigned int loadCubemap(std::vector<std::string> faces);

    // Utils() = delete;
};