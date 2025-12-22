#include <iostream>
#include <fstream>
#include <string>
#include <memory>
#include <cstdlib>  // для srand, rand
#include <ctime>    // для time
#include "scanner.h"
#include "parser.h"
#include "semantic.h"
#include "error_handler.h"

using namespace std;

void printHelp() {
    cout << "Использование:\n";
    cout << "  ./switch_translator [опции] [файл]\n\n";
    cout << "Опции:\n";
    cout << "  -h, --help       Показать эту справку\n";
    cout << "  -i, --interactive Запустить интерактивный режим\n";
    cout << "  -v N, --verbose N Уровень детализации:\n";
    cout << "                     N=0 — только результат (по умолчанию)\n";
    cout << "                     N=1 — + список токенов после сканирования\n";
    cout << "                     N=2 — + AST и таблица символов (если доступно)\n";
    cout << "  -a, --auto       Автоматический режим: генерировать код без подтверждения\n";
    cout << "\nПримеры:\n";
    cout << "  ./switch_translator examples/example1.txt\n";
    cout << "  ./switch_translator -a -v 2 examples/example1.txt\n";
    cout << "  ./switch_translator -i\n";
}

void runInteractiveMode(int verbose, bool autoMode) {
    cout << "=== ИНТЕРАКТИВНЫЙ РЕЖИМ ===\n";
    cout << "Введите программу на языке switch-lang (окончание — пустая строка или Ctrl+D):\n\n";

    string input;
    string line;

    while (getline(cin, line)) {
        if (line.empty()) break;
        input += line + "\n";
    }

    if (input.empty()) {
        cout << "Ввод отсутствует. Завершение.\n";
        return;
    }

    // Генерация I
    static bool first = true;
    if (first) {
        srand(static_cast<unsigned>(time(nullptr)));
        first = false;
    }
    int switchValue = rand() % 8;

    cout << "\n[!] Сгенерировано I = " << switchValue << "\n";

    ErrorHandler::getInstance().clear();
    Scanner scanner(input);
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

    if (verbose >= 2) {
        cout << "\n=== АБСТРАКТНОЕ СИНТАКСИЧЕСКОЕ ДЕРЕВО ===\n";
        ast->print();
    }

    // Вывод результата
    cout << "\n=== РЕЗУЛЬТАТ ВЫПОЛНЕНИЯ ===\n";

    if (!autoMode) {
        cout << "Подтвердите выполнение? [y/N]: ";
        char confirm;
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Выполнение отменено.\n";
            return;
        }
    }

    semantic.execute(ast, switchValue);
}
///============================
void processFile(const string& filename, int verbose, bool autoMode) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "[Ошибка] Не удалось открыть файл: " << filename << endl;
        return;
    }

    // Генерация I
    static bool first = true;
    if (first) {
        srand(static_cast<unsigned>(time(nullptr)));
        first = false;
    }
    int switchValue = rand() % 8;

    if (verbose >= 1) {
        cout << "[ℹ] I = " << switchValue << endl;
    }

    ErrorHandler::getInstance().clear();
    Scanner scanner(file);
    Parser parser(scanner);
    auto ast = parser.parse();

    if (ErrorHandler::getInstance().hasErrors()) {
        ErrorHandler::getInstance().printErrors();
        return;
    }

    if (verbose >= 1) {
        cout << "✓ Синтаксический анализ успешен\n";
    }

    SemanticAnalyzer semantic;
    semantic.analyze(ast);

    if (ErrorHandler::getInstance().hasErrors()) {
        ErrorHandler::getInstance().printErrors();
        return;
    }

    if (verbose >= 1) {
        cout << "✓ Семантический анализ успешен\n";
    }

    if (verbose >= 2) {
        cout << "\n=== АБСТРАКТНОЕ СИНТАКСИЧЕСКОЕ ДЕРЕВО ===\n";
        ast->print();
    }

    // Вывод результата
    if (verbose >= 1) {
        cout << "\n=== РЕЗУЛЬТАТ ВЫПОЛНЕНИЯ ===\n";
    }

    if (!autoMode) {
        cout << "Подтвердите выполнение? [y/N]: ";
        char confirm;
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Выполнение отменено.\n";
            return;
        }
    }

    semantic.execute(ast, switchValue);
}
///============================
int main(int argc, char* argv[]) {
    string filename;
    int verbose = 0;      // -v 0 — по умолчанию
    bool interactive = false;
    bool autoMode = false; // -a

    // Парсинг аргументов
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printHelp();
            return 0;
        } else if (arg == "-i" || arg == "--interactive") {
            interactive = true;
        } else if (arg == "-a" || arg == "--auto") {
            autoMode = true;
        } else if (arg == "-v" || arg == "--verbose") {
            if (i + 1 < argc) {
                try {
                    verbose = stoi(argv[++i]);
                    if (verbose < 0 || verbose > 2) {
                        cerr << "[Предупреждение] Уровень verbose должен быть 0, 1 или 2. Используется 0.\n";
                        verbose = 0;
                    }
                } catch (...) {
                    cerr << "[Ошибка] Некорректное значение для -v: " << argv[i] << endl;
                    return 1;
                }
            } else {
                cerr << "[Ошибка] Отсутствует значение после -v\n";
                printHelp();
                return 1;
            }
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            cerr << "[Ошибка] Неизвестный аргумент: " << arg << endl;
            printHelp();
            return 1;
        }
    }

    // Запуск
    if (interactive) {
        runInteractiveMode(verbose, autoMode);
    } else if (!filename.empty()) {
        processFile(filename, verbose, autoMode);
    } else {
        cout << "Укажите файл или используйте -i для интерактивного режима.\n";
        printHelp();
        return 1;
    }

    return 0;
}