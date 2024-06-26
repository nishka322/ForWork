#pragma once
#include <string>

/**
 * @brief Функция считывания строки из стандартного ввода.
 * @return Строка, считанная из стандартного ввода.
 */
std::string ReadLine();

/**
 * @brief Функция считывания целого числа из стандартного ввода.
 * @details После считывания числа выполняется считывание и отбрасывание остаточных символов во входном потоке.
 * @return Считанное целое число.
 */
int ReadLineWithNumber();
