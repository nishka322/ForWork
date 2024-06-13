#include "search_server.h"

/**
 * @brief Добавляет документ в поисковую систему.
 * @param document_id Уникальный идентификатор документа.
 * @param document Текст документа.
 * @param status Статус документа.
 * @param ratings Вектор рейтингов документа.
 * @throws invalid_argument Если document_id меньше нуля или уже существует,
 *                           или если document содержит недопустимые символы.
 */
void SearchServer::AddDocument(int document_id, const std::string& document, DocumentStatus status,
                               const std::vector<int>& ratings) {
    if ((document_id < 0) || documents_.count(document_id)) {
        throw std::invalid_argument("Document id less than zero or already exists");
    }

    const std::vector<std::string> words = SplitIntoWordsNoStop(document);
    const double inv_word_count = 1.0 / words.size();

    for (const std::string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
    }

    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids.push_back(document_id);
}

/**
 * @brief Поиск топовых документов по запросу с указанным статусом.
 * @param raw_query Необработанный запрос.
 * @param status Статус документа для поиска.
 * @return Вектор документов, найденных по запросу с указанным статусом.
 * @throws invalid_argument Если запрос содержит недопустимые символы.
 */
std::vector<Document> SearchServer::FindTopDocuments(const std::string& raw_query, DocumentStatus status) const {
    if (!IsValidWord(raw_query)) {
        throw std::invalid_argument("Invalid word in FindTopDocuments function");
    }

    auto predicate = [status](int document_id, DocumentStatus doc_status, int rating) {
        return doc_status == status;
    };

    return FindTopDocuments(raw_query, predicate);
}

/**
 * @brief Возвращает количество документов в поисковой системе.
 * @return Количество документов.
 */
int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

/**
 * @brief Находит слова запроса, совпадающие с документом по идентификатору.
 * @param raw_query Необработанный запрос.
 * @param document_id Идентификатор документа.
 * @return Кортеж, содержащий вектор совпадающих слов запроса и статус документа.
 * @throws invalid_argument Если запрос содержит недопустимые символы.
 */
std::tuple<std::vector<std::string>, DocumentStatus> SearchServer::MatchDocument(const std::string& raw_query,
                                                                                 int document_id) const {
    if (!IsValidWord(raw_query)) {
        throw std::invalid_argument("Invalid word in MatchDocument function");
    }

    const Query query = ParseQuery(raw_query);
    std::vector<std::string> matched_words;

    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(word);
        }
    }

    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }

    return std::make_tuple(matched_words, documents_.at(document_id).status);
}

/**
 * @brief Возвращает идентификатор документа по его индексу.
 * @param index Индекс документа.
 * @return Идентификатор документа.
 */
int SearchServer::GetDocumentId(const int index) const {
    return document_ids.at(index);
}

/**
 * @brief Проверяет, является ли слово стоп-словом.
 * @param word Слово для проверки.
 * @return true, если слово является стоп-словом, иначе false.
 */
bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

/**
 * @brief Считывает слова из поискового запроса и удаляет стоп-слова.
 * @param text Текст поискового запроса.
 * @return Вектор слов запроса без стоп-слов.
 */
std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Invalid word in SplitIntoWordsNoStop function");
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

/**
 * @brief Вычисляет средний рейтинг для документа на основе входного вектора рейтингов.
 * @param ratings Вектор рейтингов документа.
 * @return Средний рейтинг документа.
 */
int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) return 0;
    return std::accumulate(ratings.begin(), ratings.end(), 0) / static_cast<int>(ratings.size());
}

/**
 * @brief Разбирает слово запроса и определяет, является ли оно минус-словом или стоп-словом.
 * @param text Текст слова запроса.
 * @return Структура QueryWord с информацией о слове.
 * @throws invalid_argument Если слово содержит недопустимые символы или имеет неверный формат минус-слова.
 */
SearchServer::QueryWord SearchServer::ParseQueryWord(std::string text) const {
    QueryWord result;
    bool is_minus = false;

    if (text[0] == '-') {
        is_minus = true;
        text = text.substr(1);
    }

    if (!IsValidWord(text)) {
        throw std::invalid_argument("Invalid word in ParseQueryWord function");
    }

    if (text.empty() || text[0] == '-') {
        throw std::invalid_argument("Invalid minus word in ParseQueryWord function");
    }

    return { text, is_minus, IsStopWord(text) };
}

/**
 * @brief Разбирает текст запроса и формирует структуру Query с плюс-словами и минус-словами.
 * @param text Текст поискового запроса.
 * @return Структура Query с плюс-словами и минус-словами.
 */
SearchServer::Query SearchServer::ParseQuery(const std::string& text) const {
    Query query;
    for (const std::string& word : SplitIntoWords(text)) {
        const QueryWord query_word = ParseQueryWord(word);
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                query.minus_words.insert(query_word.data);
            } else {
                query.plus_words.insert(query_word.data);
            }
        }
    }
    return query;
}

/**
 * @brief Вычисляет обратную частоту документа для слова.
 * @param word Слово для вычисления.
 * @return Значение IDF (inverse document frequency).
 */
double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return std::log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}


