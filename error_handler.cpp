#include "error_handler.h"
#include <iostream>

using namespace std;

ErrorHandler& ErrorHandler::getInstance() {
    static ErrorHandler instance;
    return instance;
}

void ErrorHandler::addError(const string& message, int line, int column) {
    errors.push_back(Error(message, line, column));
}

void ErrorHandler::addError(const Token& token, const string& message) {
    errors.push_back(Error(message, token.line, token.column));
}

bool ErrorHandler::hasErrors() const {
    return !errors.empty();
}

void ErrorHandler::printErrors() const {
    if (errors.empty()) {
        cout << "Ошибок не обнаружено.\n";
        return;
    }
    
    cout << "\n=== ОБНАРУЖЕНЫ ОШИБКИ ===\n";
    for (const auto& error : errors) {
        cout << "[Строка " << error.line << ", Колонка " << error.column 
             << "]: " << error.message << endl;
    }
    cout << "=========================\n";
}

void ErrorHandler::clear() {
    errors.clear();
}