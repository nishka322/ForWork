/**
 * @file string_processing.h
 * @brief Содержит функции для обработки строк.
 */

#pragma once

#include <set>
#include <vector>
#include <string>

/**
 * @brief Разбивает входной текст на слова.
 *
 * Функция разбивает входной текст на отдельные слова с использованием разделителей пробелов (' ').
 *
 * @param text Входной текст для разбиения на слова.
 * @return std::vector<std::string> Вектор, содержащий отдельные слова из входного текста.
 */
std::vector<std::string> SplitIntoWords(const std::string& text);

/**
 * @brief Создает множество уникальных непустых строк из коллекции строк.
 *
 * Функция перебирает каждую строку в коллекции @p strings и добавляет ее в множество
 * @c non_empty_strings, если строка не пустая. Дубликаты игнорируются, так как множество
 * содержит только уникальные элементы.
 *
 * @tparam StringCollection Тип коллекции строк, например, std::vector<std::string> или std::set<std::string>.
 * @param strings Коллекция строк, из которой нужно создать множество уникальных непустых строк.
 * @return std::set<std::string> Множество, содержащее уникальные непустые строки из коллекции @p strings.
 */
template <typename StringCollection>
std::set<std::string> MakeUniqueNonEmptyStrings(const StringCollection& strings) {
    std::set<std::string> non_empty_strings;
    for (const std::string& str : strings) {
        if (!str.empty()) {
            non_empty_strings.insert(str);
        }
    }
    return non_empty_strings;
}
