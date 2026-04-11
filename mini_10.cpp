#include <iostream>
#include <exception>

using namespace std;

template <typename T, size_t limit>
struct InstancesCounter {
	static size_t alive_instances;

	InstancesCounter() {
		if (alive_instances >= limit) {
			throw runtime_error("alive instances amount exceeded");
		}
		++alive_instances;
	}

	InstancesCounter(const InstancesCounter&) {
		if (alive_instances >= limit) {
			throw runtime_error("alive instances amount exceeded");
		}
		++alive_instances;
	}

protected:
	~InstancesCounter() {
		--alive_instances;
	}
};

template <typename T, size_t limit> size_t InstancesCounter<T, limit>::alive_instances(0);

template <typename T, typename U>
struct Pair : InstancesCounter<Pair<T, U>, 3> {
	T first;
	U second;

	Pair(T a, U b) : first(a), second(b) {}
};


int main() {

	Pair<int, int> foo1{1, 1};
	Pair<int, int> foo2{2, 2};

	Pair<int, float> bar{2, 2};

	cout << InstancesCounter<Pair<int, int>, 3>::alive_instances << '\n';
	cout << InstancesCounter<Pair<int, float>, 3>::alive_instances << '\n';


	Pair<int, int> foo3{3, 3};
	Pair<int, int> foo4{4, 4};
	cout << InstancesCounter<Pair<int, int>, 3>::alive_instances << '\n';

	return 0;
}
