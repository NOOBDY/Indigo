#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include "pch.hpp"

#include <exception>

class FileNotFoundException : public std::exception {
public:
    FileNotFoundException(std::string filename);

    const char *what() const noexcept override;

private:
    std::string m_Filename;
};

#endif