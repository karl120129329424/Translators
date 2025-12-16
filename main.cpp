#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include "scanner.h"
#include "parser.h"
#include "semantic.h"
#include "error_handler.h"

using namespace std;

void printHelp() {
    cout << "Использование:\n";
    cout << "  switch_translator [опции] [файл]\n\n";
    cout << "Опции:\n";
    cout << "  -h, --help       Показать эту справку\n";
    cout << "  -i, --interactive Интерактивный режим\n";
    cout << "  -v, --value N    Установить значение переменной I (по умолчанию: 1)\n";
    cout << "  -a, --ast        Показать AST\n";
    cout << "  -s, --symbols    Показать таблицу символов\n";
}

void runInteractiveMode() {
    cout << "=== ИНТЕРАКТИВНЫЙ РЕЖИМ ===" << endl;
    cout << "Введите оператор switch (Ctrl+D для завершения):\n\n";
    
    string input;
    string line;
    
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) {
            break;
        }
        
        if (line == "exit" || line == "quit") {
            break;
        }
        
        input += line + "\n";
        
        // Проверяем, завершен ли оператор switch
        if (line.find('}') != string::npos) {
            // Очищаем предыдущие ошибки
            ErrorHandler::getInstance().clear();
            
            Scanner scanner(input);
            Parser parser(scanner);
            
            auto ast = parser.parse();
            
            if (ErrorHandler::getInstance().hasErrors()) {
                ErrorHandler::getInstance().printErrors();
            } else {
                cout << "\n✓ Синтаксический анализ успешен\n";
                
                SemanticAnalyzer semantic;
                semantic.analyze(ast);
                
                if (ErrorHandler::getInstance().hasErrors()) {
                    ErrorHandler::getInstance().printErrors();
                } else {
                    cout << "✓ Семантический анализ успешен\n";
                    
                    // Запрашиваем значение для выполнения
                    int switchValue;
                    cout << "\nВведите значение переменной I: ";
                    if (cin >> switchValue) {
                        cin.ignore(); // Очищаем буфер
                        semantic.execute(ast, switchValue);
                    } else {
                        cout << "Некорректное значение\n";
                        cin.clear();
                        cin.ignore(10000, '\n');
                    }
                }
            }
            
            input.clear();
            cout << endl;
        }
    }
}

void processFile(const string& filename, int switchValue, bool showAST, bool showSymbols) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return;
    }
    
    cout << "=== ОБРАБОТКА ФАЙЛА: " << filename << " ===" << endl;
    
    Scanner scanner(file);
    Parser parser(scanner);
    
    auto ast = parser.parse();
    
    if (ErrorHandler::getInstance().hasErrors()) {
        ErrorHandler::getInstance().printErrors();
        return;
    }
    
    cout << "✓ Синтаксический анализ успешен\n";
    
    SemanticAnalyzer semantic;
    semantic.analyze(ast);
    
    if (ErrorHandler::getInstance().hasErrors()) {
        ErrorHandler::getInstance().printErrors();
        return;
    }
    
    cout << "✓ Семантический анализ успешен\n";
    
    if (showAST) {
        cout << "\n=== АБСТРАКТНОЕ СИНТАКСИЧЕСКОЕ ДЕРЕВО ===" << endl;
        ast->print();
    }
    
    if (showSymbols) {
        semantic.printSymbolTable();
    }
    
    cout << "\n=== РЕЗУЛЬТАТ ВЫПОЛНЕНИЯ ===" << endl;
    semantic.execute(ast, switchValue);
}

int main(int argc, char* argv[]) {
    string filename;
    int switchValue = 1;
    bool interactive = false;
    bool showAST = false;
    bool showSymbols = false;
    
    // Парсинг аргументов командной строки
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-i" || arg == "--interactive") {
            interactive = true;
        } else if (arg == "-v" || arg == "--value") {
            if (i + 1 < argc) {
                try {
                    switchValue = stoi(argv[++i]);
                } catch (...) {
                    cerr << "Ошибка: некорректное значение для -v" << endl;
                    return 1;
                }
            } else {
                cerr << "Ошибка: отсутствует значение для -v" << endl;
                return 1;
            }
        } else if (arg == "-a" || arg == "--ast") {
            showAST = true;
        } else if (arg == "-s" || arg == "--symbols") {
            showSymbols = true;
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            cerr << "Неизвестный аргумент: " << arg << endl;
            printHelp();
            return 1;
        }
    }
    
    if (interactive) {
        runInteractiveMode();
    } else if (!filename.empty()) {
        processFile(filename, switchValue, showAST, showSymbols);
    } else {
        cout << "Введите оператор switch (пустая строка для завершения):\n\n";
        
        string input;
        string line;
        
        cout << "> ";
        while (getline(cin, line) && !line.empty()) {
            input += line + "\n";
            cout << "> ";
        }
        
        if (!input.empty()) {
            ErrorHandler::getInstance().clear();
            
            Scanner scanner(input);
            Parser parser(scanner);
            
            auto ast = parser.parse();
            
            if (ErrorHandler::getInstance().hasErrors()) {
                ErrorHandler::getInstance().printErrors();
            } else {
                cout << "\n✓ Синтаксический анализ успешен\n";
                
                SemanticAnalyzer semantic;
                semantic.analyze(ast);
                
                if (ErrorHandler::getInstance().hasErrors()) {
                    ErrorHandler::getInstance().printErrors();
                } else {
                    cout << "✓ Семантический анализ успешен\n";
                    
                    cout << "\nВведите значение переменной I: ";
                    if (cin >> switchValue) {
                        semantic.execute(ast, switchValue);
                    } else {
                        cout << "Некорректное значение, используется значение по умолчанию: 1\n";
                        semantic.execute(ast, 1);
                    }
                }
            }
        }
    }
    
    return 0;
}