#ifndef SCANNER_H
#define SCANNER_H

#include <string>
#include <vector>
#include <fstream>

// Типы токенов
enum class TokenType {
    // Ключевые слова
    SWITCH,
    CASE,
    DEFAULT,
    BREAK,
    PRINT,
    
    // Идентификаторы и константы
    IDENTIFIER,      // I
    NUMBER,         // N (для номеров case)
    STRING_LITERAL, // "Текст"
    
    // Символы
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )
    LEFT_BRACE,     // {
    RIGHT_BRACE,    // }
    COLON,          // :
    SEMICOLON,      // ;
    
    // Специальные
    END_OF_FILE,
    ERROR
};

// Структура токена
struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t = TokenType::ERROR, std::string l = "", int ln = 0, int col = 0)
        : type(t), lexeme(l), line(ln), column(col) {}
};

class Scanner {
public:
    Scanner(const std::string& input);
    Scanner(std::ifstream& file);
    ~Scanner();
    
    Token getNextToken();
    Token peekToken();
    bool hasMoreTokens() const;
    void reset();
    
private:
    std::string input;
    size_t position;
    size_t line;
    size_t column;
    size_t start;
    
    char advance();
    char peek() const;
    bool isAtEnd() const;
    void skipWhitespace();
    void skipComment();
    
    Token makeToken(TokenType type) const;
    Token makeToken(TokenType type, const std::string& lexeme) const;
    Token errorToken(const std::string& message) const;
    
    Token scanIdentifierOrKeyword();
    Token scanNumber();
    Token scanString();
};

#endif // SCANNER_H