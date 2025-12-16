#include "parser.h"
#include "error_handler.h"
#include <iostream>
#include <iomanip>

using namespace std;

void SwitchNode::print(int indent) const {
    cout << string(indent, ' ') << "SWITCH (I) {" << endl;
    for (const auto& caseNode : cases) {
        caseNode->print(indent + 2);
    }
    if (defaultCase) {
        defaultCase->print(indent + 2);
    }
    cout << string(indent, ' ') << "}" << endl;
}

void CaseNode::print(int indent) const {
    cout << string(indent, ' ') << "CASE " << value.lexeme << ":" << endl;
    for (const auto& action : actions) {
        action->print(indent + 2);
    }
    cout << string(indent, ' ') << "BREAK;" << endl;
}

void DefaultNode::print(int indent) const {
    cout << string(indent, ' ') << "DEFAULT:" << endl;
    for (const auto& action : actions) {
        action->print(indent + 2);
    }
}

void PrintNode::print(int indent) const {
    cout << string(indent, ' ') << "print(\"" << text.lexeme << "\");" << endl;
}

Parser::Parser(Scanner& scanner) : scanner(scanner) {
    advance();
}

void Parser::advance() {
    previousToken = currentToken;
    currentToken = scanner.getNextToken();
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::check(TokenType type) const {
    return currentToken.type == type;
}

Token Parser::consume(TokenType type, const string& errorMessage) {
    if (check(type)) {
        Token token = currentToken;
        advance();
        return token;
    }
    
    ErrorHandler::getInstance().addError(currentToken, errorMessage);
    synchronize();
    return Token(type, "", currentToken.line, currentToken.column);
}

void Parser::synchronize() {
    while (!check(TokenType::END_OF_FILE)) {
        if (previousToken.type == TokenType::SEMICOLON) return;
        
        switch (currentToken.type) {
            case TokenType::SWITCH:
            case TokenType::CASE:
            case TokenType::DEFAULT:
            case TokenType::PRINT:
                return;
            default:
                break;
        }
        
        advance();
    }
}

unique_ptr<ASTNode> Parser::parse() {
    return parseProgram();
}

unique_ptr<ASTNode> Parser::parseProgram() {
    // <Программа> ::= <Оператор>
    return parseOperator();
}

unique_ptr<SwitchNode> Parser::parseOperator() {
    // <Оператор> ::= SWITCH (I) {<СписокКейсов> <ПоУмолчанию>}
    auto switchNode = make_unique<SwitchNode>();
    
    consume(TokenType::SWITCH, "Ожидается ключевое слово 'switch'");
    consume(TokenType::LEFT_PAREN, "Ожидается '(' после 'switch'");
    
    Token varToken = consume(TokenType::IDENTIFIER, "Ожидается переменная 'I'");
    if (varToken.lexeme != "I") {
        ErrorHandler::getInstance().addError(varToken, 
            "Ожидается переменная 'I', получено: " + varToken.lexeme);
    }
    switchNode->variable = varToken;
    
    consume(TokenType::RIGHT_PAREN, "Ожидается ')' после переменной");
    consume(TokenType::LEFT_BRACE, "Ожидается '{' после switch(I)");
    
    // Парсим список case
    switchNode->cases = parseCaseList();
    
    // Парсим default
    switchNode->defaultCase = parseDefault();
    
    consume(TokenType::RIGHT_BRACE, "Ожидается '}' в конце оператора switch");
    
    return switchNode;
}

vector<unique_ptr<ASTNode>> Parser::parseCaseList() {
    // <СписокКейсов> ::= <СписокКейсов> <Кейс> | <Кейс>
    vector<unique_ptr<ASTNode>> cases;
    
    while (check(TokenType::CASE)) {
        cases.push_back(parseCase());
    }
    
    return cases;
}

unique_ptr<CaseNode> Parser::parseCase() {
    // <Кейс> ::= CASE I : <СписокДействий> BREAK ;
    auto caseNode = make_unique<CaseNode>();
    
    consume(TokenType::CASE, "Ожидается ключевое слово 'case'");
    
    caseNode->value = consume(TokenType::NUMBER, "Ожидается число после 'case'");
    consume(TokenType::COLON, "Ожидается ':' после номера case");
    
    // Парсим список действий
    caseNode->actions = parseActionList();
    
    consume(TokenType::BREAK, "Ожидается 'break' в конце case");
    consume(TokenType::SEMICOLON, "Ожидается ';' после 'break'");
    
    return caseNode;
}

unique_ptr<DefaultNode> Parser::parseDefault() {
    // <ПоУмолчанию> ::= DEFAULT : <СписокДействий>
    auto defaultNode = make_unique<DefaultNode>();
    
    consume(TokenType::DEFAULT, "Ожидается ключевое слово 'default'");
    consume(TokenType::COLON, "Ожидается ':' после 'default'");
    
    // Парсим список действий
    defaultNode->actions = parseActionList();
    
    return defaultNode;
}

vector<unique_ptr<ASTNode>> Parser::parseActionList() {
    // <СписокДействий> ::= <СписокДействий> <Действие> | <Действие>
    vector<unique_ptr<ASTNode>> actions;
    
    while (check(TokenType::PRINT)) {
        actions.push_back(parseAction());
    }
    
    return actions;
}

unique_ptr<PrintNode> Parser::parseAction() {
    // <Действие> ::= print ( "Текст" ) ;
    auto printNode = make_unique<PrintNode>();
    
    consume(TokenType::PRINT, "Ожидается 'print'");
    consume(TokenType::LEFT_PAREN, "Ожидается '(' после 'print'");
    
    printNode->text = consume(TokenType::STRING_LITERAL, "Ожидается строковая константа");
    
    consume(TokenType::RIGHT_PAREN, "Ожидается ')' после строки");
    consume(TokenType::SEMICOLON, "Ожидается ';' после print()");
    
    return printNode;
}