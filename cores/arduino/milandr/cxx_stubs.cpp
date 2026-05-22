#include <stdlib.h>

// Оператор delete для C++
void operator delete(void *ptr) noexcept {
    free(ptr);
}

void operator delete[](void *ptr) noexcept {
    free(ptr);
}

// Опционально: если нужен placement delete
void operator delete(void *ptr, unsigned int size) noexcept {
    free(ptr);
}