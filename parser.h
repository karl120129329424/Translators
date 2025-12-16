#ifndef PARSER_H
#define PARSER_H

#include "scanner.h"
#include <vector>
#include <memory>
#include <string>

// Абстрактный узел AST
struct ASTNode {
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
};

// Узел для оператора switch
struct SwitchNode : public ASTNode {
    Token variable;
    std::vector<std::unique_ptr<ASTNode>> cases;
    std::unique_ptr<ASTNode> defaultCase;
    
    void print(int indent = 0) const override;
};

// Узел для case
struct CaseNode : public ASTNode {
    Token value;
    std::vector<std::unique_ptr<ASTNode>> actions;
    
    void print(int indent = 0) const override;
};

// Узел для default
struct DefaultNode : public ASTNode {
    std::vector<std::unique_ptr<ASTNode>> actions;
    
    void print(int indent = 0) const override;
};

// Узел для оператора print
struct PrintNode : public ASTNode {
    Token text;
    
    void print(int indent = 0) const override;
};

class Parser {
public:
    Parser(Scanner& scanner);
    
    std::unique_ptr<ASTNode> parse();
    
private:
    Scanner& scanner;
    Token currentToken;
    Token previousToken;
    
    void advance();
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& errorMessage);
    
    // Функции разбора для каждого нетерминала
    /*
    Грамматика (вариант 18):
    <Программа> ::= <Оператор>
    <Оператор> ::= SWITCH (I) {<СписокКейсов> <ПоУмолчанию>}
    <СписокКейсов> ::= <СписокКейсов> <Кейс> | <Кейс>
    <Кейс> ::= CASE I : <СписокДействий> BREAK ;
    <ПоУмолчанию> ::= DEFAULT : <СписокДействий>
    <СписокДействий> ::= <СписокДействий> <Действие> | <Действие>
    <Действие> ::= print ( "Текст" ) ;
    */
    
    std::unique_ptr<ASTNode> parseProgram();
    std::unique_ptr<SwitchNode> parseOperator();
    std::vector<std::unique_ptr<ASTNode>> parseCaseList();
    std::unique_ptr<CaseNode> parseCase();
    std::unique_ptr<DefaultNode> parseDefault();
    std::vector<std::unique_ptr<ASTNode>> parseActionList();
    std::unique_ptr<PrintNode> parseAction();
    
    void synchronize();
};

#endif // PARSER_H