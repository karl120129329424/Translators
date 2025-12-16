#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include "scanner.h"

struct Error {
    std::string message;
    int line;
    int column;
    
    Error(const std::string& msg, int ln, int col)
        : message(msg), line(ln), column(col) {}
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    void addError(const std::string& message, int line, int column);
    void addError(const Token& token, const std::string& message);
    bool hasErrors() const;
    void printErrors() const;
    void clear();
    
private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    
    std::vector<Error> errors;
};

#endif // ERROR_HANDLER_H