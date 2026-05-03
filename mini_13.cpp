#include <iostream>

using namespace std;

template <typename Checker, typename... Args>
int getIndexOfFirstMatch(Checker check_fn, Args... args) {
	if constexpr (sizeof...(args) == 0) {
		return -1;
	}

	int res = -1;
	int index = 0;

	auto foo = [&](auto x) {
		bool status = check_fn(x);
		if (status) {
			res = index;
		}
		++index;

		return status;
	};

	(foo(args) || ...);
	return res;
}

signed main() {

	auto checker = [&](auto x) {
		return x > 0;
	};

	cout << getIndexOfFirstMatch(checker, -1, -2, 3.1, 4) << endl;
	cout << getIndexOfFirstMatch(checker, -1, -2, -3, -4) << endl;

	return 0;
}
