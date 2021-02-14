#include <algorithm>
#include <atomic>
#include <cmath>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <thread>
#include <valarray>
#include <vector>

/**
 * L個のm乗数でnumberを表せる組み合わせの数を返す関数
 */
std::vector<std::valarray<uintmax_t>>
get_patterns(const uintmax_t number, const uintmax_t L, const uintmax_t m) {
	uintmax_t count = 0;

	std::vector<std::valarray<uintmax_t>> patterns;
	std::valarray<uintmax_t>              pattern(1, L);
	while (true) {
		auto sum = std::transform_reduce(
		    std::execution::unseq, std::begin(pattern), std::end(pattern),
		    (uintmax_t)0, std::plus<uintmax_t>(),
		    [m](uintmax_t value) { return std::pow(value, m); });
		if (number == sum) {
			++count;
			patterns.push_back(pattern);
		}

		if (sum >= number) {
			/* 繰り上がり */
			auto it = std::begin(pattern) + 1;
			do {
				++*it;
				std::fill(std::execution::unseq, std::begin(pattern), it, *it);
				sum = std::transform_reduce(
				    std::execution::unseq, std::begin(pattern), std::end(pattern),
				    (uintmax_t)0, std::plus<uintmax_t>(),
				    [m](uintmax_t value) { return std::pow(value, m); });
				++it;
			} while (it != std::end(pattern) && sum > number);

			if (it == std::end(pattern) && sum > number) {
				break;
			}
		} else {
			pattern[0]++;
		}
	}

	return patterns;
}

int main() {
	uintmax_t L = 2;
	uintmax_t m = 3;
	uintmax_t n = 2;
	std::cin >> L >> m >> n;

	std::vector<std::thread> threads;
	const unsigned int       thread_num =
	    std::max(std::thread::hardware_concurrency(), 8u);

	std::vector<std::valarray<uintmax_t>> patterns;

	//	std::atomic<uintmax_t> number = 27223627;
	std::atomic<uintmax_t> number = 0;
	bool                   found  = false;
	for (unsigned int thread_i = 0; thread_i < thread_num; ++thread_i) {
		threads.emplace_back([&] {
			while (!found) {
				auto this_patterns = get_patterns(++number, L, m);
				if (n == this_patterns.size()) {
					found    = true;
					patterns = this_patterns;
					break;
				}
			}
		});
	}
	for (auto &thread : threads) {
		thread.join();
	}

	/* 結果表示 */
	number = std::pow(*patterns.cbegin(), m).sum();
	std::cout << number << std::endl;

	for (const auto &pattern : patterns) {
		for (const auto &number : pattern) {
			std::cout << number << ", ";
		}
		std::cout << std::endl;
	}
}
