#include <string>
#include <vector>
#include <sstream>  // для std::stringstream
#include <cctype>   // для std::isspace

/**
 * @brief Разделяет строку на отдельные слова на основе пробельных символов.
 * @param text Входная строка для разделения.
 * @return Вектор слов, извлеченных из входной строки.
 */
std::vector<std::string> SplitIntoWords(const std::string& text) {
    std::vector<std::string> words;
    std::stringstream ss(text);
    std::string word;

    while (ss >> word) {
        words.push_back(word);
    }

    return words;
}
