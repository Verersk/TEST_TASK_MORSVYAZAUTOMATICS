#include <atomic>
#include <array>
#include <cstddef>

template <typename T, std::size_t Capacity>
class CircularQueue {
private:
    std::array<T, Capacity> buffer_;
    std::atomic<std::size_t> head_{0};  // Индекс считывания 
    std::atomic<std::size_t> tail_{0};  // Индекс записи

public:
    CircularQueue() = default;

    // Конструктор копирования, не thread-safe, не использовать в двух потоках. 
    CircularQueue(const CircularQueue& otherQueue) {
        this->buffer_ = otherQueue.buffer_;
        this->head_.store(otherQueue.head_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        this->tail_.store(otherQueue.tail_.load(std::memory_order_relaxed), std::memory_order_relaxed);
    }

    // Перегрузка оператора присваивания, не thread-safe, не использовать в двух потоках.
    CircularQueue& operator=(const CircularQueue& otherQueue) {
        if (this != &otherQueue) {
             this->buffer_ = otherQueue.buffer_;
             this->head_.store(otherQueue.head_.load(std::memory_order_relaxed), std::memory_order_relaxed);
             this->tail_.store(otherQueue.tail_.load(std::memory_order_relaxed), std::memory_order_relaxed);
        }
        return *this;
    }

    // Добавляем элемент в очередь (вызывается producer)
    bool push(const T& value) {
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        std::size_t next_tail = (tail + 1) % Capacity;
        if (next_tail == head_.load(std::memory_order_acquire)) {
            return false;  // Очередь заполнена
        }
        buffer_[tail] = value;
        tail_.store(next_tail, std::memory_order_release);
        return true;
    }

    // Извлекаем элемент из очереди (вызывается consumer)
    bool pop(T& value) {
        std::size_t head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire)) {
            return false;  // Очередь пуста
        }
        value = std::move(buffer_[head]);
        head_.store((head + 1) % Capacity, std::memory_order_release);
        return true;
    }

    // Проверяем пуста-ли очередь (не thread-safe, для отладки или использования в одном потоке)
    bool empty() const {
        return head_.load(std::memory_order_relaxed) == tail_.load(std::memory_order_relaxed);
    }

    // Проверяем заполнение очереди (не thread-safe,  для отладки или использования в одном потоке)
    bool full() const {
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        std::size_t next_tail = (tail + 1) % Capacity;
        return next_tail == head_.load(std::memory_order_relaxed);
    }

    // Получить текущий размер (для отладки или использования в одном потоке)
    std::size_t size() const {
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        return (tail >= head) ? (tail - head) : (Capacity - head + tail);
    }
};