#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

class CitationError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

#endif