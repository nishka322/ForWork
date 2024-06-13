#include "request_queue.h"

/**
 * @brief Добавляет запрос на поиск документов с указанным запросом и статусом.
 * @param raw_query Необработанный запрос.
 * @param status Статус документа для поиска (по умолчанию ACTUAL).
 * @return Вектор документов, найденных по запросу.
 */
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    const auto result = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(result.size());
    return result;
}

/**
 * @brief Добавляет запрос на поиск документов с указанным запросом.
 * @param raw_query Необработанный запрос.
 * @return Вектор документов, найденных по запросу.
 */
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    const auto result = search_server_.FindTopDocuments(raw_query);
    AddRequest(result.size());
    return result;
}

/**
 * @brief Возвращает количество запросов без результатов.
 * @return Количество запросов без результатов.
 */
int RequestQueue::GetNoResultRequests() const {
    return no_results_requests_;
}

/**
 * @brief Добавляет новый запрос в очередь и обновляет статистику.
 * @param results_num Количество результатов поиска для текущего запроса.
 */
void RequestQueue::AddRequest(int results_num) {
    // Новый запрос - новая секунда
    ++current_time_;

    // Удаляем устаревшие результаты поиска
    while (!requests_.empty() && min_in_day_ <= current_time_ - requests_.front().timestamp) {
        if (0 == requests_.front().results) {
            --no_results_requests_;
        }
        requests_.pop_front();
    }

    // Сохраняем новый результат поиска
    requests_.push_back({current_time_, results_num});
    if (0 == results_num) {
        ++no_results_requests_;
    }
}
