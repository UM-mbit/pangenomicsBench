#ifndef BARRIER_H
#define BARRIER_H

const char* __attribute__((noinline)) __begin_pin_roi(const char *s, int *beg, int *end);

const char* __attribute__((noinline)) __end_pin_roi(const char *s, int *beg, int *end);

template <class T>
__attribute__((always_inline)) inline void Barrier(const T &value) {
    asm volatile("" : "+m"(const_cast<T &>(value)));
}


#define BEGIN_PIN_ROI __begin_pin_roi(new char[5], new int, new int);
#define END_PIN_ROI __end_pin_roi(new char[5], new int, new int);


#endif //BARRIER_H
