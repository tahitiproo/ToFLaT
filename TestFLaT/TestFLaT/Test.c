#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Возвращает:
// 1 - если скобочная последовательность верна
// 0 - если неверна
// -1 - в случае ошибки (например, не удалось выделить память)
int checkBracket(const char* str) {
    size_t len = strlen(str);
    if (len == 0) {
        return 1; // Пустая строка считается корректной.
    }
    char* stack = (char*)malloc(len);          // Выделяем память под стек. 
    //В худшем случае (строка из одних открывающих скобок) размер стека будет равен длине строки.S
    if (stack == NULL) {
        fprintf(stderr, "Ошибка: не удалось выделить память.\n");
        return -1; // Возвращаем код ошибки.
    }
    int top = -1; // Указывает на вершину стека.

    for (size_t i = 0; i < len; i++) {
        char c = str[i];

        switch (c) {
        case '(':
        case '[':
        case '{':
            // Добавляем открывающую скобку в стек.
            stack[++top] = c;
            break;
        case ')':
            // Если стек пуст или на вершине не та скобка - ошибка.
            if (top == -1 || stack[top] != '(') {
                free(stack);
                return 0; // Неверная последовательность.
            }
            top--; // Снимаем со стека парную скобку.
            break;
        case ']':
            if (top == -1 || stack[top] != '[') {
                free(stack);
                return 0;
            }
            top--;
            break;
        case '}':
            if (top == -1 || stack[top] != '{') {
                free(stack);
                return 0;
            }
            top--;
            break;
        default:
            // Игнорируем любые другие символы (буквы, цифры и т.д.).
            break;
        } // Если в конце стек оказался пуст, значит, для каждой открывающей
    }     // скобки нашлась парная закрывающая.
    int result = (top == -1);
    free(stack);

    return result;
}
//Пример использования
int main() {
    const char* test1 = "({[]})";      // Корректно
    const char* test2 = "({[}])";      // Некорректно
    const char* test3 = "())";         // Некорректно
    const char* test4 = "([{}])abc";   // Корректно (игнорируем abc)

    printf("Test1 (%s): %s\n", test1, checkBracket(test1) ? "true" : "false");
    printf("Test2 (%s): %s\n", test2, checkBracket(test2) ? "true" : "false");
    printf("Test3 (%s): %s\n", test3, checkBracket(test3) ? "true" : "false");
    printf("Test4 (%s): %s\n", test4, checkBracket(test4) ? "true" : "false");

    return 0;

}