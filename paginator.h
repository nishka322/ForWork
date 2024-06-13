#pragma once
#include <vector>
#include <algorithm> // для std::min

using namespace std::string_literals;

/**
 * @brief Класс для работы с диапазоном итераторов.
 * @tparam Iterator Тип итератора.
 */
template <typename Iterator>
class IteratorRange {
public:
    /**
     * @brief Конструктор с параметрами.
     * @param begin Итератор начала диапазона.
     * @param end Итератор конца диапазона.
     */
    IteratorRange(Iterator begin, Iterator end)
            : first_(begin)
            , last_(end)
            , size_(distance(first_, last_)) {
    }

    /**
     * @brief Возвращает итератор начала диапазона.
     * @return Итератор начала диапазона.
     */
    Iterator begin() const {
        return first_;
    }

    /**
     * @brief Возвращает итератор конца диапазона.
     * @return Итератор конца диапазона.
     */
    Iterator end() const {
        return last_;
    }

    /**
     * @brief Возвращает размер диапазона (количество элементов).
     * @return Размер диапазона.
     */
    size_t size() const {
        return size_;
    }

private:
    Iterator first_; ///< Итератор начала диапазона.
    Iterator last_;  ///< Итератор конца диапазона.
    size_t size_;    ///< Размер диапазона.
};

/**
 * @brief Оператор вывода для класса IteratorRange.
 * @tparam Iterator Тип итератора.
 * @param out Поток вывода.
 * @param range Диапазон итераторов.
 * @return Поток вывода.
 */
template <typename Iterator>
std::ostream& operator<<(std::ostream& out, const IteratorRange<Iterator>& range) {
    for (Iterator it = range.begin(); it != range.end(); ++it) {
        out << *it;
    }
    return out;
}

/**
 * @brief Класс для разбиения контейнера на страницы с итераторами.
 * @tparam Iterator Тип итератора.
 */
template <typename Iterator>
class Paginator {
public:
    /**
     * @brief Конструктор с параметрами.
     * @param begin Итератор начала контейнера.
     * @param end Итератор конца контейнера.
     * @param page_size Размер страницы (количество элементов на странице).
     */
    Paginator(Iterator begin, Iterator end, size_t page_size) {
        for (size_t left = distance(begin, end); left > 0;) {
            const size_t current_page_size = std::min(page_size, left);
            const Iterator current_page_end = next(begin, current_page_size);
            pages_.push_back({begin, current_page_end});

            left -= current_page_size;
            begin = current_page_end;
        }
    }

    /**
     * @brief Возвращает итератор начала последовательности страниц.
     * @return Итератор начала последовательности страниц.
     */
    auto begin() const {
        return pages_.begin();
    }

    /**
     * @brief Возвращает итератор конца последовательности страниц.
     * @return Итератор конца последовательности страниц.
     */
    auto end() const {
        return pages_.end();
    }

    /**
     * @brief Возвращает количество страниц.
     * @return Количество страниц.
     */
    size_t size() const {
        return pages_.size();
    }

private:
    std::vector<IteratorRange<Iterator>> pages_; ///< Вектор диапазонов итераторов страниц.
};

/**
 * @brief Функция для создания объекта класса Paginator на основе контейнера.
 * @tparam Container Тип контейнера.
 * @param c Константная ссылка на контейнер.
 * @param page_size Размер страницы (количество элементов на странице).
 * @return Объект класса Paginator.
 */
template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}
