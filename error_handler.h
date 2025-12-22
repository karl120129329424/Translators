#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <string>
#include <vector>
#include "scanner.h"

enum class ErrorType {
    LEXICAL,
    SYNTAX,
    SEMANTIC
};

struct Error {
    ErrorType type;
    std::string message;
    int line;
    int column;
    
    Error(ErrorType t, const std::string& msg, int ln, int col)
        : type(t), message(msg), line(ln), column(col) {}
};

class ErrorHandler {
public:
    static ErrorHandler& getInstance();
    
    void addLexicalError(const std::string& message, int line, int column);
    void addSyntaxError(const std::string& message, int line, int column);
    void addSemanticError(const std::string& message, int line, int column);
    
    void addLexicalError(const Token& token, const std::string& message);
    void addSyntaxError(const Token& token, const std::string& message);
    void addSemanticError(const Token& token, const std::string& message);
    
    bool hasErrors() const;
    void printErrors() const;
    void clear();
    
private:
    ErrorHandler() = default;
    ~ErrorHandler() = default;
    
    std::vector<Error> errors;
};

#endif // ERROR_HANDLER_H