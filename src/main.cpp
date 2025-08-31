#include <iostream>
#include <type_traits>
#include <stdexcept>
#include "circleQueue.h"
#include <thread>
#include <chrono>

using namespace std;

// Функция производителя: записывает числа в очередь
void producer(CircularQueue<int, 10>& queue) {
    for (int i = 0; i < 15; ++i) {
        if (queue.push(i)) {
            cout << "Producer: Pushed " << i << endl;
        } else {
            cout << "Producer: Queue full, failed to push " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Ждём, чтобы consumer освободил место
            --i; // Повторить попытку
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Имитация работы
    }
}

// Функция потребителя: читает числа из очереди
void consumer(CircularQueue<int, 10>& queue) {
    int value;
    for (int i = 0; i < 15; ++i) {
        if (queue.pop(value)) {
            cout << "Consumer: Popped " << value << endl;
        } else {
            cout << "Consumer: Queue empty, failed to pop" << endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Ждём, чтобы producer добавил данные
            --i; // Повторить попытку
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(75)); // Имитация работы
    }
}




int main() {

    CircularQueue<int, 10> queue; // Очередь на 10 элементов типа int

    // Запускаем два потока: producer и consumer
    thread producer_thread(producer, ref(queue));
    thread consumer_thread(consumer, ref(queue));

    // Ждём завершения потоков
    producer_thread.join();
    consumer_thread.join();

    // Проверяем состояние очереди (не thread-safe, но здесь безопасно, так как потоки завершены)
    cout << "Final queue size: " << queue.size() << endl;
    cout << "Queue empty: " << (queue.empty() ? "true" : "false") << endl;

    return 0;
}