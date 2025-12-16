#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "parser.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();
    
    void analyze(std::unique_ptr<ASTNode>& ast);
    void execute(std::unique_ptr<ASTNode>& ast, int switchValue);
    void printSymbolTable() const;
    
private:
    std::unordered_map<int, std::vector<std::string>> caseMap; // номер case -> список действий
    
    void analyzeSwitchNode(SwitchNode* node);
    void analyzeCaseNode(CaseNode* node);
    void analyzeDefaultNode(DefaultNode* node);
    void analyzePrintNode(PrintNode* node);
    
    void executeSwitchNode(SwitchNode* node, int switchValue);
    void executeCaseNode(CaseNode* node, int caseValue);
    void executePrintNode(PrintNode* node);
    
    bool validateCaseValue(const Token& token);
    bool validateVariable(const Token& token);
};

#endif // SEMANTIC_H