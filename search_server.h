#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "document.h"
#include "read_input_functions.h"
#include "string_processing.h"

/**
 * @brief Класс SearchServer для поисковой системы.
 */
class SearchServer {
public:
    /**
     * @brief Конструктор класса SearchServer.
     * @tparam StringContainer Тип контейнера со строками (например, std::vector<std::string>).
     * @param stop_words Контейнер со стоп-словами для инициализации.
     * @throws invalid_argument Если какое-либо стоп-слово содержит недопустимые символы.
     */
    template <typename StringContainer>
    explicit SearchServer(const StringContainer& stop_words);

    /**
     * @brief Конструктор класса SearchServer.
     * @param stop_words_text Текст со стоп-словами для инициализации.
     */
    explicit SearchServer(const std::string& stop_words_text)
            : SearchServer(SplitIntoWords(stop_words_text)) {}

    // Методы поисковой системы

    /**
     * @brief Добавляет документ в поисковую систему.
     * @param document_id Уникальный идентификатор документа.
     * @param document Текст документа.
     * @param status Статус документа.
     * @param ratings Вектор рейтингов документа.
     * @throws invalid_argument Если document_id меньше нуля или уже существует,
     *                           или если document содержит недопустимые символы.
     */
    void AddDocument(int document_id, const std::string& document, DocumentStatus status,
                     const std::vector<int>& ratings);

    /**
     * @brief Поиск топовых документов по запросу с указанным статусом.
     * @param raw_query Необработанный запрос.
     * @param status Статус документа для поиска (по умолчанию DocumentStatus::ACTUAL).
     * @return Вектор документов, найденных по запросу с указанным статусом.
     * @throws invalid_argument Если запрос содержит недопустимые символы.
     */
    std::vector<Document> FindTopDocuments(const std::string& raw_query, DocumentStatus status = DocumentStatus::ACTUAL) const;

    /**
     * @brief Поиск топовых документов по запросу с заданным предикатом.
     * @tparam predicate Тип предиката для фильтрации документов.
     * @param raw_query Необработанный запрос.
     * @param predict Предикат для фильтрации документов.
     * @return Вектор документов, отфильтрованных и отсортированных по релевантности.
     * @throws invalid_argument Если запрос содержит недопустимые символы.
     */
    template<typename predicate>
    std::vector<Document> FindTopDocuments(const std::string& raw_query, predicate predict) const;

    /**
     * @brief Возвращает количество документов в поисковой системе.
     * @return Количество документов.
     */
    int GetDocumentCount() const;

    /**
     * @brief Находит слова запроса, совпадающие с документом по идентификатору.
     * @param raw_query Необработанный запрос.
     * @param document_id Идентификатор документа.
     * @return Кортеж, содержащий вектор совпадающих слов запроса и статус документа.
     * @throws invalid_argument Если запрос содержит недопустимые символы.
     */
    std::tuple<std::vector<std::string>, DocumentStatus> MatchDocument(const std::string& raw_query, int document_id) const;

    /**
     * @brief Возвращает идентификатор документа по его индексу.
     * @param index Индекс документа.
     * @return Идентификатор документа.
     */
    int GetDocumentId(const int index) const;

private:
    struct DocumentData {
        int rating;             ///< Рейтинг документа.
        DocumentStatus status;  ///< Статус документа.
    };

    std::set<std::string> stop_words_;                           ///< Множество стоп-слов.
    std::map<std::string, std::map<int, double>> word_to_document_freqs_;  ///< Частота слов в документах.
    std::map<int, DocumentData> documents_;                      ///< Документы в поисковой системе.
    std::vector<int> document_ids;                               ///< Идентификаторы документов.

    /**
     * @brief Проверяет, является ли слово стоп-словом.
     * @param word Слово для проверки.
     * @return true, если слово является стоп-словом, иначе false.
     */
    bool IsStopWord(const std::string& word) const;

    /**
     * @brief Считывает слова поискового запроса и удаляет из него стоп-слова.
     * @param text Текст поискового запроса.
     * @return Вектор слов запроса без стоп-слов.
     */
    std::vector<std::string> SplitIntoWordsNoStop(const std::string& text) const;

    /**
     * @brief Вычисляет средний рейтинг документа на основе вектора рейтингов.
     * @param ratings Вектор рейтингов документа.
     * @return Средний рейтинг документа.
     */
    static int ComputeAverageRating(const std::vector<int>& ratings);

    /**
     * @brief Структура для представления слова запроса.
     */
    struct QueryWord {
        std::string data;   ///< Данные слова запроса.
        bool is_minus;      ///< Является ли слово минус-словом.
        bool is_stop;       ///< Является ли слово стоп-словом.
    };

    /**
     * @brief Разбирает слово запроса и определяет его тип (плюс, минус или стоп).
     * @param text Текст слова запроса.
     * @return Структура QueryWord с информацией о слове.
     */
    QueryWord ParseQueryWord(std::string text) const;

    /**
     * @brief Структура для представления запроса.
     */
    struct Query {
        std::set<std::string> plus_words;   ///< Множество плюс-слов запроса.
        std::set<std::string> minus_words;  ///< Множество минус-слов запроса.
    };

    /**
     * @brief Разбирает текст запроса и формирует структуру Query с плюс- и минус-словами.
     * @param text Текст поискового запроса.
     * @return Структура Query с плюс- и минус-словами.
     */
    Query ParseQuery(const std::string& text) const;

    /**
     * @brief Вычисляет обратную частоту документа для слова.
     * @param word Слово для вычисления IDF.
     * @return Значение IDF (inverse document frequency).
     */
    double ComputeWordInverseDocumentFreq(const std::string& word) const;

    /**
     * @brief Проверяет, является ли слово допустимым для использования в поисковом запросе.
     * @param word Слово для проверки.
     * @return true, если слово допустимо, иначе false.
     */
    static bool IsValidWord(const std::string& word);

    /**
     * @brief Возвращает все документы, соответствующие запросу и предикату.
     * @tparam DocPredicate Тип предиката для фильтрации документов.
     * @param query Запрос.
     * @param doc_pred Предикат для фильтрации документов.
     * @return Вектор всех документов, удовлетворяющих запросу и предикату.
     */
    template<typename DocPredicate>
    std::vector<Document> FindAllDocuments(const Query& query, DocPredicate doc_pred) const;
};

template <typename StringContainer>
SearchServer::SearchServer(const StringContainer& stop_words)
        : stop_words_(MakeUniqueNonEmptyStrings(stop_words)) {
    // Проверяем каждое стоп-слово на допустимость
    for(const auto& stop_word: stop_words_){
        if(!IsValidWord(stop_word)){
            throw std::invalid_argument("invalid word in class constructor");
        }
    }
}

template<typename predicate>
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, predicate predict) const {
    // Проверяем валидность запроса
    if(!IsValidWord(raw_query)){
        throw std::invalid_argument("Invalid word in FindTopDocument function");
    }

    // Парсим запрос
    const Query query = ParseQuery(raw_query);

    // Находим все документы, удовлетворяющие запросу и предикату
    auto matched_documents = FindAllDocuments(query, predict);

    // Сортируем документы по релевантности и рейтингу
    std::sort(matched_documents.begin(), matched_documents.end(),
              [](const Document& lhs, const Document& rhs) {
                  if (std::abs(lhs.relevance - rhs.relevance) < std::numeric_limits<double>::epsilon()) {
                      return lhs.rating > rhs.rating;
                  }
                  return lhs.relevance > rhs.relevance;
              });

    // Ограничиваем количество возвращаемых документов
    if (matched_documents.size() > MAX_RESULT_DOCUMENT_COUNT) {
        matched_documents.resize(MAX_RESULT_DOCUMENT_COUNT);
    }

    return matched_documents;
}

template<typename DocPredicate>
std::vector<Document> SearchServer::FindAllDocuments(const Query& query, DocPredicate doc_pred) const {
    // Карта для хранения релевантности каждого документа
    std::map<int, double> document_to_relevance;

    // Вычисляем релевантность для плюс-слов
    for(const std::string& word : query.plus_words) {
        if(word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        const double inverse_document_freq = ComputeWordInverseDocumentFreq(word);

        for(const auto& [document_id, term_freq] : word_to_document_freqs_.at(word)) {
            const auto& document_info = documents_.at(document_id);
            if(doc_pred(document_id, document_info.status, document_info.rating)) {
                document_to_relevance[document_id] += term_freq * inverse_document_freq;
            }
        }
    }

    // Удаляем документы, соответствующие минус-словам
    for(const std::string& word : query.minus_words) {
        if(word_to_document_freqs_.count(word) == 0) {
            continue;
        }

        for(const auto& [document_id, _] : word_to_document_freqs_.at(word)) {
            document_to_relevance.erase(document_id);
        }
    }

    // Преобразуем карту в вектор документов и возвращаем его
    std::vector<Document> matched_documents;
    for(const auto& [document_id, relevance] : document_to_relevance) {
        matched_documents.push_back({document_id, relevance, documents_.at(document_id).rating});
    }

    return matched_documents;
}

/**
 * @brief Проверяет, является ли слово допустимым для использования в поисковом запросе.
 * @param word Слово для проверки.
 * @return true, если слово допустимо, иначе false.
 */
bool IsValidWord(const std::string& word)  {
    return std::none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}