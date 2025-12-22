#include "error_handler.h"
#include <iostream>

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::addLexicalError(const std::string& message, int line, int column) {
    errors.emplace_back(ErrorType::LEXICAL, message, line, column);
}

void ErrorHandler::addSyntaxError(const std::string& message, int line, int column) {
    errors.emplace_back(ErrorType::SYNTAX, message, line, column);
}

void ErrorHandler::addSemanticError(const std::string& message, int line, int column) {
    errors.emplace_back(ErrorType::SEMANTIC, message, line, column);
}

void ErrorHandler::addLexicalError(const Token& token, const std::string& message) {
    addLexicalError(message, token.line, token.column);
}

void ErrorHandler::addSyntaxError(const Token& token, const std::string& message) {
    addSyntaxError(message, token.line, token.column);
}

void ErrorHandler::addSemanticError(const Token& token, const std::string& message) {
    addSemanticError(message, token.line, token.column);
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

void ErrorHandler::printErrors() const {
    for (const auto& err : errors) {
        std::string typeStr;
        switch (err.type) {
            case ErrorType::LEXICAL:  typeStr = "Лексическая ошибка"; break;
            case ErrorType::SYNTAX:    typeStr = "Синтаксическая ошибка"; break;
            case ErrorType::SEMANTIC:  typeStr = "Семантическая ошибка"; break;
        }
        std::cerr << "[" << typeStr << "] в строке " << err.line 
                  << ", колонка " << err.column << ":\n"
                  << err.message << "\n\n";
    }
}

void ErrorHandler::clear() {
    errors.clear();
}