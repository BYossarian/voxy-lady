
#include <string>
#include <fstream>

std::string readFile(const std::string &filePath) {

    // ref: http://insanecoding.blogspot.com/2011/11/how-to-read-in-file-in-c.html
    // ref: https://stackoverflow.com/a/2602258/1937302
    // ref: http://en.cppreference.com/w/cpp/io/c#Binary_and_text_modes
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Couldn't open file: " + filePath);
    }

    // get size of file:
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // create std::string object of correct size:
    std::string contents(fileSize, ' ');

    // read in data:
    file.read(&contents[0], contents.size());
    file.close();

    return contents;

}
