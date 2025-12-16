#include "semantic.h"
#include "error_handler.h"
#include <iostream>
#include <algorithm>
#include <memory>

using namespace std;

SemanticAnalyzer::SemanticAnalyzer() {
    caseMap.clear();
}

void SemanticAnalyzer::analyze(unique_ptr<ASTNode>& ast) {
    if (!ast) return;
    
    SwitchNode* switchNode = dynamic_cast<SwitchNode*>(ast.get());
    if (switchNode) {
        analyzeSwitchNode(switchNode);
    }
}

void SemanticAnalyzer::analyzeSwitchNode(SwitchNode* node) {
    // Проверяем переменную
    if (!validateVariable(node->variable)) {
        ErrorHandler::getInstance().addError(node->variable,
            "В операторе switch может использоваться только переменная 'I'");
    }
    
    // Проверяем все case
    vector<int> caseValues;
    for (auto& caseNodePtr : node->cases) {
        CaseNode* caseNode = dynamic_cast<CaseNode*>(caseNodePtr.get());
        if (caseNode) {
            analyzeCaseNode(caseNode);
            
            // Проверяем уникальность значений case
            int value = stoi(caseNode->value.lexeme);
            if (find(caseValues.begin(), caseValues.end(), value) != caseValues.end()) {
                ErrorHandler::getInstance().addError(caseNode->value,
                    "Повторяющееся значение case: " + caseNode->value.lexeme);
            }
            caseValues.push_back(value);
        }
    }
    
    // Проверяем default, если есть
    if (node->defaultCase) {
        DefaultNode* defaultNode = dynamic_cast<DefaultNode*>(node->defaultCase.get());
        if (defaultNode) {
            analyzeDefaultNode(defaultNode);
        }
    }
    
    // Сохраняем информацию для выполнения
    for (auto& caseNodePtr : node->cases) {
        CaseNode* caseNode = dynamic_cast<CaseNode*>(caseNodePtr.get());
        if (caseNode) {
            int value = stoi(caseNode->value.lexeme);
            vector<string> actions;
            for (auto& action : caseNode->actions) {
                PrintNode* printNode = dynamic_cast<PrintNode*>(action.get());
                if (printNode) {
                    actions.push_back(printNode->text.lexeme);
                }
            }
            caseMap[value] = actions;
        }
    }
}

void SemanticAnalyzer::analyzeCaseNode(CaseNode* node) {
    // Проверяем значение case
    if (!validateCaseValue(node->value)) {
        ErrorHandler::getInstance().addError(node->value,
            "Недопустимое значение case: " + node->value.lexeme);
    }
    
    // Проверяем действия
    for (auto& action : node->actions) {
        PrintNode* printNode = dynamic_cast<PrintNode*>(action.get());
        if (printNode) {
            analyzePrintNode(printNode);
        }
    }
    
    // Проверяем, что есть хотя бы одно действие
    if (node->actions.empty()) {
        ErrorHandler::getInstance().addError(node->value,
            "Case должен содержать хотя бы одно действие");
    }
}

void SemanticAnalyzer::analyzeDefaultNode(DefaultNode* node) {
    // Проверяем действия
    for (auto& action : node->actions) {
        PrintNode* printNode = dynamic_cast<PrintNode*>(action.get());
        if (printNode) {
            analyzePrintNode(printNode);
        }
    }
    
    // Проверяем, что есть хотя бы одно действие
    if (node->actions.empty()) {
        ErrorHandler::getInstance().addError(Token(), 
            "Default должен содержать хотя бы одно действие");
    }
}

void SemanticAnalyzer::analyzePrintNode(PrintNode* node) {
    // Проверяем, что строка не пустая
    if (node->text.lexeme.empty()) {
        ErrorHandler::getInstance().addError(node->text,
            "Строка в print() не может быть пустой");
    }
}

bool SemanticAnalyzer::validateCaseValue(const Token& token) {
    if (token.type != TokenType::NUMBER) return false;
    
    try {
        int value = stoi(token.lexeme);
        return value >= 0; // Можно добавить дополнительные ограничения
    } catch (...) {
        return false;
    }
}

bool SemanticAnalyzer::validateVariable(const Token& token) {
    return token.type == TokenType::IDENTIFIER && token.lexeme == "I";
}

void SemanticAnalyzer::execute(unique_ptr<ASTNode>& ast, int switchValue) {
    if (!ast) {
        cout << "Ошибка: AST пуст\n";
        return;
    }
    
    SwitchNode* switchNode = dynamic_cast<SwitchNode*>(ast.get());
    if (switchNode) {
        executeSwitchNode(switchNode, switchValue);
    }
}

void SemanticAnalyzer::executeSwitchNode(SwitchNode* node, int switchValue) {
    cout << "\n=== ВЫПОЛНЕНИЕ SWITCH ===" << endl;
    cout << "Значение переменной I = " << switchValue << endl;
    
    bool caseFound = false;
    
    // Ищем подходящий case
    for (auto& caseNodePtr : node->cases) {
        CaseNode* caseNode = dynamic_cast<CaseNode*>(caseNodePtr.get());
        if (caseNode) {
            int caseValue = stoi(caseNode->value.lexeme);
            if (caseValue == switchValue) {
                caseFound = true;
                executeCaseNode(caseNode, caseValue);
                break;
            }
        }
    }
    
    // Если case не найден, выполняем default
    if (!caseFound && node->defaultCase) {
        DefaultNode* defaultNode = dynamic_cast<DefaultNode*>(node->defaultCase.get());
        if (defaultNode) {
            cout << "Выполняется default:" << endl;
            for (auto& action : defaultNode->actions) {
                PrintNode* printNode = dynamic_cast<PrintNode*>(action.get());
                if (printNode) {
                    executePrintNode(printNode);
                }
            }
        }
    } else if (!caseFound) {
        cout << "Не найден подходящий case и отсутствует default\n";
    }
}

void SemanticAnalyzer::executeCaseNode(CaseNode* node, int caseValue) {
    cout << "Выполняется case " << caseValue << ":" << endl;
    for (auto& action : node->actions) {
        PrintNode* printNode = dynamic_cast<PrintNode*>(action.get());
        if (printNode) {
            executePrintNode(printNode);
        }
    }
}

void SemanticAnalyzer::executePrintNode(PrintNode* node) {
    cout << "  Вывод: " << node->text.lexeme << endl;
}

void SemanticAnalyzer::printSymbolTable() const {
    cout << "\n=== ТАБЛИЦА СИМВОЛОВ ===" << endl;
    for (const auto& entry : caseMap) {
        cout << "Case " << entry.first << ": ";
        for (const auto& action : entry.second) {
            cout << "print(\"" << action << "\") ";
        }
        cout << endl;
    }
    cout << "========================" << endl;
}