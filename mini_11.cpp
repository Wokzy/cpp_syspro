#include <iostream>

using false_class = std::integral_constant<bool, false>;
using true_class = std::integral_constant<bool, true>;

template<int N, int S>
struct PrimeCheker: std::integral_constant<bool, std::conditional_t<(N % S) == 0, false_class, PrimeCheker<N, S - 1>>{}> {};
template<int N> struct PrimeCheker<N, 1>: std::integral_constant<bool, true_class{}> {};

template<int N>
struct IsPrime: PrimeCheker<N, N - 1> {};
template<> struct IsPrime<1>: std::integral_constant<bool, true_class{}> {};

template<int N>
struct NextPrime: std::integral_constant<int, std::conditional_t<IsPrime<N + 1>{}, std::integral_constant<int, N + 1>, NextPrime<N + 1>>{}> {};

template<int N>
struct NthPrime: std::integral_constant<int, NextPrime<NthPrime<N - 1>{}>{}> {};

template<> struct NthPrime<0>: std::integral_constant<int, 1> {};


int main() {
	std::cout << NthPrime<100>{} << std::endl;

	return 0;
}
