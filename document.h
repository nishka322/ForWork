#pragma once
#include <iostream>

using namespace std::string_literals;

const int MAX_RESULT_DOCUMENT_COUNT = 5;

/**
 * @brief Структура, представляющая документ.
 */
struct Document {
    /**
     * @brief Конструктор по умолчанию.
     */
    Document() = default;

    /**
     * @brief Конструктор с параметрами.
     * @param id Идентификатор документа.
     * @param relevance Релевантность документа.
     * @param rating Рейтинг документа.
     */
    Document(int id, double relevance, int rating)
            : id(id)
            , relevance(relevance)
            , rating(rating) {
    }

    int id = 0; ///< Идентификатор документа.
    double relevance = 0.0; ///< Релевантность документа.
    int rating = 0; ///< Рейтинг документа.
};

/**
 * @brief Перегрузка оператора вывода для структуры Document.
 * @param out Поток вывода.
 * @param document Документ, который нужно вывести.
 * @return Поток вывода.
 */
std::ostream& operator<<(std::ostream& out, const Document& document) {
    out << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return out;
}

/**
 * @brief Перечисление статусов документа.
 */
enum class DocumentStatus {
    ACTUAL,        ///< Актуальный
    IRRELEVANT,    ///< Устаревший
    BANNED,        ///< Отклонённый
    REMOVED        ///< Удалённый
};
