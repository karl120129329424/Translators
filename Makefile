# Компилятор и флаги
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -O2

# Имена исполняемого файла и объектных файлов
TARGET = switch_translator
OBJS = main.o scanner.o parser.o semantic.o error_handler.o

# Правило по умолчанию
all: $(TARGET)

# Сборка исполняемого файла
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Компиляция отдельных модулей
main.o: main.cpp scanner.h parser.h semantic.h error_handler.h
	$(CXX) $(CXXFLAGS) -c main.cpp

scanner.o: scanner.cpp scanner.h
	$(CXX) $(CXXFLAGS) -c scanner.cpp

parser.o: parser.cpp parser.h scanner.h error_handler.h
	$(CXX) $(CXXFLAGS) -c parser.cpp

semantic.o: semantic.cpp semantic.h parser.h error_handler.h
	$(CXX) $(CXXFLAGS) -c semantic.cpp

error_handler.o: error_handler.cpp error_handler.h scanner.h
	$(CXX) $(CXXFLAGS) -c error_handler.cpp

# Очистка
clean:
	rm -f $(OBJS) $(TARGET)

# Запуск тестов
test: $(TARGET)
	./$(TARGET) examples/example1.txt

test-interactive: $(TARGET)
	./$(TARGET) -i

test-ast: $(TARGET)
	./$(TARGET) -a examples/example1.txt

test-value: $(TARGET)
	./$(TARGET) -v 2 examples/example1.txt

# Справка
help:
	@echo "Доступные цели:"
	@echo "  all           - сборка программы (по умолчанию)"
	@echo "  clean         - удаление объектных файлов и исполняемого файла"
	@echo "  test          - запуск теста с example1.txt"
	@echo "  test-interactive - запуск в интерактивном режиме"
	@echo "  test-ast      - запуск с выводом AST"
	@echo "  test-value    - запуск с указанием значения переменной"
	@echo "  help          - вывод этой справки"

.PHONY: all clean test test-interactive test-ast test-value help