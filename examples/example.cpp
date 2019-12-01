#include <iostream>
#include <string>
#include "logger.h"
#include <type_traits>
#include <experimental/type_traits>

template<typename T, typename std::enable_if_t<!std::is_integral<T>::value>* = nullptr>
void fun()
{
    LOG << "General";
}

template<typename T, typename std::enable_if_t<std::is_integral<T>::value>* = nullptr>
void fun()
{
    LOG << "integral";
}

template<typename T>
using can_a = decltype(&T::a);

struct A
{
    void a() {}
};

struct NoA {};

template <typename T>
using Can_a = std::experimental::is_detected<can_a, T>;
template <typename T, typename std::enable_if_t<Can_a<T>::value, T>* = nullptr >
void f(T t){
    t.a();
}
int main()
{
    static_assert(std::experimental::is_detected<can_a, A>::value, "can_a function required");
    static_assert(!std::experimental::is_detected<can_a, NoA>::value, "can_a function required");
    fun<float>();
    fun<int>();
    fun<const char*>();
    f(A{});
}

