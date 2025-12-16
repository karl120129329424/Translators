#include "scanner.h"
#include <cctype>
#include <unordered_map>

using namespace std;

// Таблица ключевых слов
static const unordered_map<string, TokenType> keywords = {
    {"switch", TokenType::SWITCH},
    {"case", TokenType::CASE},
    {"default", TokenType::DEFAULT},
    {"break", TokenType::BREAK},
    {"print", TokenType::PRINT}
};

Scanner::Scanner(const string& input) 
    : input(input), position(0), line(1), column(1), start(0) {}

Scanner::Scanner(ifstream& file) : position(0), line(1), column(1), start(0) {
    // Чтение всего файла в строку
    input = string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

Scanner::~Scanner() {}

char Scanner::advance() {
    if (isAtEnd()) return '\0';
    char c = input[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

char Scanner::peek() const {
    if (isAtEnd()) return '\0';
    return input[position];
}

bool Scanner::isAtEnd() const {
    return position >= input.length();
}

void Scanner::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/') {
            skipComment();
        } else {
            break;
        }
    }
}

void Scanner::skipComment() {
    if (peek() == '/' && position + 1 < input.length() && input[position + 1] == '/') {
        // Однострочный комментарий
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
        if (!isAtEnd()) advance(); // Пропускаем \n
    } else if (peek() == '/' && position + 1 < input.length() && input[position + 1] == '*') {
        // Многострочный комментарий
        advance(); // /
        advance(); // *
        while (!isAtEnd() && !(peek() == '*' && position + 1 < input.length() && input[position + 1] == '/')) {
            advance();
        }
        if (!isAtEnd()) advance(); // *
        if (!isAtEnd()) advance(); // /
    }
}

Token Scanner::makeToken(TokenType type) const {
    string lexeme = input.substr(start, position - start);
    return Token(type, lexeme, line, column - lexeme.length());
}

Token Scanner::makeToken(TokenType type, const string& lexeme) const {
    return Token(type, lexeme, line, column - lexeme.length());
}

Token Scanner::errorToken(const string& message) const {
    return Token(TokenType::ERROR, message, line, column);
}

Token Scanner::scanIdentifierOrKeyword() {
    while (!isAtEnd() && (isalnum(peek()) || peek() == '_')) {
        advance();
    }
    
    string lexeme = input.substr(start, position - start);
    
    auto it = keywords.find(lexeme);
    if (it != keywords.end()) {
        return makeToken(it->second);
    }
    
    // Проверяем, если это переменная I
    if (lexeme == "I") {
        return makeToken(TokenType::IDENTIFIER, lexeme);
    }
    
    return errorToken("Недопустимый идентификатор: " + lexeme);
}

Token Scanner::scanNumber() {
    while (!isAtEnd() && isdigit(peek())) {
        advance();
    }
    
    // Проверяем, что это только число (без точки)
    string lexeme = input.substr(start, position - start);
    return makeToken(TokenType::NUMBER, lexeme);
}

Token Scanner::scanString() {
    advance(); // Пропускаем первую кавычку
    
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\\') {
            advance(); // Пропускаем escape-символ
        }
        advance();
    }
    
    if (isAtEnd()) {
        return errorToken("Незавершенная строковая константа");
    }
    
    advance(); // Пропускаем закрывающую кавычку
    
    string lexeme = input.substr(start + 1, position - start - 2);
    return makeToken(TokenType::STRING_LITERAL, lexeme);
}

Token Scanner::getNextToken() {
    skipWhitespace();
    
    start = position;
    
    if (isAtEnd()) {
        return makeToken(TokenType::END_OF_FILE, "");
    }
    
    char c = advance();
    
    // Проверяем отдельные символы
    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case ':': return makeToken(TokenType::COLON);
        case ';': return makeToken(TokenType::SEMICOLON);
        case '"': return scanString();
    }
    
    // Числа
    if (isdigit(c)) {
        position--; // Возвращаемся назад, чтобы scanNumber мог начать правильно
        return scanNumber();
    }
    
    // Идентификаторы и ключевые слова
    if (isalpha(c) || c == '_') {
        return scanIdentifierOrKeyword();
    }
    
    return errorToken(string("Неизвестный символ: ") + c);
}

Token Scanner::peekToken() {
    size_t savedPos = position;
    size_t savedLine = line;
    size_t savedCol = column;
    size_t savedStart = start;
    
    Token token = getNextToken();
    
    position = savedPos;
    line = savedLine;
    column = savedCol;
    start = savedStart;
    
    return token;
}

bool Scanner::hasMoreTokens() const {
    size_t savedPos = position;
    while (savedPos < input.length()) {
        char c = input[savedPos];
        if (c != ' ' && c != '\t' && c != '\r' && c != '\n') {
            return true;
        }
        savedPos++;
    }
    return false;
}

void Scanner::reset() {
    position = 0;
    line = 1;
    column = 1;
    start = 0;
}