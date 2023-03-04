#include "exception.hpp"

FileNotFoundException::FileNotFoundException(std::string filename) {
    m_Filename = "Failed Loading File: '" + filename + "'";
}

const char *FileNotFoundException::what() const noexcept {
    return m_Filename.c_str();
}