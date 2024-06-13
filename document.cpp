#include "document.h"

/**
 * @brief Вывод информации о документе в стандартный поток вывода.
 * @param document Ссылка на объект типа Document, который нужно вывести.
 */
void PrintDocument(const Document& document) {
    std::cout << "{ "s
              << "document_id = "s << document.id << ", "s
              << "relevance = "s << document.relevance << ", "s
              << "rating = "s << document.rating << " }"s << std::endl;
}