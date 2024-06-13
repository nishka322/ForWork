#include "read_input_functions.h"
#include <iostream>

/**
 * @brief Функция считывания строки из стандартного ввода.
 * @return Строка, считанная из стандартного ввода.
 */
std::string ReadLine() {
    std::string s;
    getline(std::cin, s);
    return s;
}

/**
 * @brief Функция считывания целого числа из стандартного ввода.
 * @details После считывания числа выполняется считывание и отбрасывание остаточных символов во входном потоке.
 * @return Считанное целое число.
 */
int ReadLineWithNumber() {
    int result;
    std::cin >> result;
    ReadLine(); // считываем и отбрасываем остаточные символы
    return result;
}
