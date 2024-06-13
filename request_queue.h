#pragma once
#include <deque>
#include <cstdint>
#include "search_server.h"

/**
 * @brief Класс для управления очередью запросов к поисковому серверу.
 */
class RequestQueue {
public:
    /**
     * @brief Конструктор класса RequestQueue.
     * @param search_server Ссылка на объект SearchServer, с которым будет работать очередь запросов.
     */
    explicit RequestQueue(const SearchServer& search_server)
            : search_server_(search_server)
            , no_results_requests_(0)
            , current_time_(0) {
    }

    /**
     * @brief Добавляет запрос на поиск документов с указанным запросом и статусом.
     * @param raw_query Необработанный запрос.
     * @param status Статус документа для поиска (по умолчанию ACTUAL).
     * @return Вектор документов, найденных по запросу.
     */
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    /**
     * @brief Добавляет запрос на поиск документов с указанным запросом.
     * @param raw_query Необработанный запрос.
     * @return Вектор документов, найденных по запросу.
     */
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    /**
     * @brief Шаблонный метод для добавления запроса на поиск документов с предикатом.
     * @tparam DocumentPredicate Тип предиката для фильтрации документов.
     * @param raw_query Необработанный запрос.
     * @param document_predicate Предикат для фильтрации документов.
     * @return Вектор документов, найденных по запросу с учётом предиката.
     */
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);

    /**
     * @brief Возвращает количество запросов без результатов.
     * @return Количество запросов без результатов.
     */
    int GetNoResultRequests() const;

private:
    /**
     * @brief Структура для хранения результата запроса и временной метки.
     */
    struct QueryResult {
        uint64_t timestamp; ///< Временная метка запроса.
        int results; ///< Количество результатов поиска.
    };

    std::deque<QueryResult> requests_; ///< Очередь запросов с результатами.
    const SearchServer& search_server_; ///< Ссылка на объект поискового сервера.
    int no_results_requests_; ///< Количество запросов без результатов.
    uint64_t current_time_; ///< Текущее время.
    const static int min_in_day_ = 1440; ///< Минут в сутках.

    /**
     * @brief Добавляет новый запрос в очередь и обновляет статистику.
     * @param results_num Количество результатов поиска для текущего запроса.
     */
    void AddRequest(int results_num);
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    const auto result = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(result.size());
    return result;
}
