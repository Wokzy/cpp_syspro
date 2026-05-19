// #include <bits/stdc++.h>
#include <stdexcept>
#include <iostream>
#include <utility>
#include <memory>
#include <chrono>
#include <thread>
#include <mutex>


using namespace std;

template<typename T>
void my_swap(T& a, T& b) {
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}

template<typename T>
class Treap {
public:
	std::mutex mtx;

	struct TreapNode {
		int priority;
		int size;
		T sum;
		T value;
		shared_ptr<TreapNode> right;
		shared_ptr<TreapNode> left;

		TreapNode(int p, T val) : sum(val), value(val), priority(p), size(1), right(nullptr), left(nullptr) {}
		TreapNode(const TreapNode &other) : priority(other.priority), sum(other.sum), size(other.size),
									value(other.value) {

			if (other.right != nullptr) {
				right = make_shared<TreapNode>(*other.right);
			}

			if (other.left != nullptr) {
				left = make_shared<TreapNode>(*other.left);
			}
		}

		TreapNode(TreapNode &&other) : priority(other.priority), sum(other.sum), size(other.size),
							value(other.value) {

			my_swap(right, other.right);
			my_swap(left, other.left);
		}

		TreapNode &operator=(const TreapNode &other) {
			priority = other.priority;
			sum = other.sum;
			size = other.size;
			value = other.value;

			if (other.right != nullptr) {
				right = make_shared<TreapNode>(*other.right);
			}

			if (other.left != nullptr) {
				left = make_shared<TreapNode>(*other.left);
			}

			return *this;
		}

		TreapNode &operator=(TreapNode &&other) {
			priority = other.priority;
			sum = other.sum;
			size = other.size;
			value = other.value;
			my_swap(right, other.right);
			my_swap(left, other.left);

			return *this;
		}

		void update() {
			size = 1;
			sum = value;
			if (left) {
				size += left->size;
				sum += left->sum;
			}
			if (right) {
				size += right->size;
				sum += right->sum;
			}
		}
	};

protected:

	static shared_ptr<TreapNode> merge(shared_ptr<TreapNode> x, shared_ptr<TreapNode> y) {
		if (x == nullptr) return y;
		if (y == nullptr) return x;

		if (x->priority < y->priority) {
			x->right = merge(x->right, y);
			x->right->update();
			x->update();
			return x;
		}

		y->left = merge(x, y->left);
		y->left->update();
		y->update();
		return y;
	}


	static pair<shared_ptr<TreapNode>, shared_ptr<TreapNode>> splitBySize(shared_ptr<TreapNode> x, int k) {
		if (x == nullptr) return {nullptr, nullptr};

		int l_size = 0;
		if (x->left) l_size = x->left->size;

		if (k <= l_size) {
			auto ll_lr = splitBySize(x->left, k);
			x->left = ll_lr.second;
			x->update();
			return {ll_lr.first, x};
		}

		auto rl_rr = splitBySize(x->right, k - l_size - 1);
		x->right = rl_rr.first;
		x->update();
		return {x, rl_rr.second};
	}


	static shared_ptr<TreapNode> insert(shared_ptr<TreapNode> x, int pos, T value) {
		auto l_r = splitBySize(x, pos - 1);
		auto y = make_shared<TreapNode>(rand(), value);
		return merge(merge(l_r.first, y), l_r.second);
	}

	static shared_ptr<TreapNode> erase(shared_ptr<TreapNode> x, int pos, int count) {
		auto l_r = splitBySize(x, pos - 1);
		auto rl_rr = splitBySize(l_r.second, count);

		return merge(l_r.first, rl_rr.second);
	}

	static shared_ptr<TreapNode> findAtPosition(shared_ptr<TreapNode> x, int pos) {
		if (x == nullptr) return nullptr;


		int l_size = 0;
		if (x->left) l_size = x->left->size;

		if (pos == l_size + 1) {
			return x;
		}

		if (pos <= l_size) {
			return findAtPosition(x->left, pos);
		}

		return findAtPosition(x->right, pos - l_size - 1);
	}


	shared_ptr<TreapNode> hp_node; // highest-priority node

public:

	friend class iterator;

	class iterator {
	protected:
		int size_ptr = 1;
		shared_ptr<TreapNode> loc_hp_node;

		void out_of_bounds_check() const {
			if (size_ptr > loc_hp_node->size)
				throw runtime_error("Iterator out of bounds");
		}

	public:

		iterator(int size_ptr, shared_ptr<TreapNode> loc_hp_node) : size_ptr(size_ptr), loc_hp_node(loc_hp_node) {
			// mtx.lock();
		}

		// ~iterator() {
		// 	mtx.unlock();
		// }

		T& operator*() {
			out_of_bounds_check();

			return findAtPosition(loc_hp_node, size_ptr)->value;
		}

		T* operator->() {
			out_of_bounds_check();

			return &findAtPosition(loc_hp_node, size_ptr)->value;
		}

		iterator &operator++() {
			size_ptr = min(size_ptr + 1, loc_hp_node->size + 1);
			return *this;
		}

		iterator operator++(int) {
			iterator tmp = *this;
			++(*this);
			return tmp;
		}

		friend bool operator==(const iterator& a, const iterator& b) {
			return (a.size_ptr == b.size_ptr) && (a.loc_hp_node == b.loc_hp_node);
		}

		friend bool operator!=(const iterator& a, const iterator& b) {
			return (a.size_ptr != b.size_ptr) || (a.loc_hp_node != b.loc_hp_node);
		}
	};

	Treap(): hp_node(nullptr) {}
	Treap(shared_ptr<TreapNode> node): hp_node(node) {}

	Treap(std::initializer_list<T> il) {
		hp_node = nullptr;

		int i = 1;
		for (auto it = il.begin(); it != il.end(); it++, i++) {
			insert(i, *it);
		}
	}

	Treap(Treap &&other) {
		std::lock_guard<std::mutex> lk{other.mtx};
		my_swap(hp_node, other.hp_node);
	}

	Treap(const Treap &other): hp_node(nullptr) {
		if (other.hp_node != nullptr) {
			std::lock_guard<std::mutex> lk{other.mtx};
			hp_node = make_shared<TreapNode>(*other.hp_node);
		}
	}

	Treap &operator=(shared_ptr<TreapNode> node) {
		std::lock_guard<std::mutex> lk{mtx};
		hp_node = node;
		return *this;
	}

	Treap &operator=(const Treap &other) {
		if (other.hp_node != nullptr) {
			std::scoped_lock sl{mtx, other.mtx};
			hp_node = make_shared<TreapNode>(*other.hp_node);
		}

		return *this;
	}

	Treap &operator=(Treap &&other) {
		std::scoped_lock sl{mtx, other.mtx};
		my_swap(hp_node, other.hp_node);

		return *this;
	}

	iterator begin() {
		throw std::runtime_error("NotImplementedError");
		// return iterator{1, hp_node};
	}

	iterator end() {
		throw std::runtime_error("NotImplementedError");
		// return iterator{1 + hp_node->size, hp_node};
	}

	T sum(int lq, int rq) {
		std::lock_guard<std::mutex> lk{mtx};

		T res{};
		if (hp_node == nullptr) {
			return res;
		}

		auto l_r = splitBySize(hp_node, lq - 1);
		auto rl_rr = splitBySize(l_r.second, rq - lq + 1);

		if (rl_rr.first != nullptr) {
			res = rl_rr.first->sum;
		}

		merge(l_r.first, merge(rl_rr.first, rl_rr.second));
		return res;
	}

	T sum() {
		if (hp_node == nullptr) {
			return T{};
		}

		return sum(0, hp_node->size);
	}

	void insert(int pos, T value) {
		// cout << "insert start\n";
		std::lock_guard<std::mutex> lk{mtx};
		hp_node = insert(hp_node, pos, value);
		// cout << "insert finish\n";
	}

	void erase(int pos, int count) {
		std::lock_guard<std::mutex> lk{mtx};
		hp_node = erase(hp_node, pos, count);
	}

	int size() {
		std::lock_guard<std::mutex> lk{mtx};
		if (hp_node == nullptr) {
			return 0;
		}

		return hp_node->size;
	}
};

struct SomeValue {
	float val;

	friend SomeValue operator+(SomeValue& a, SomeValue& b) {
		return {a.val + b.val};
	}

	SomeValue &operator+=(SomeValue& other) {
		val += other.val;
		return *this;
	}
};



std::atomic<bool> finished = false;
std::mutex cout_lock;

void producer(Treap<int> &treap, int num_iterations) {
	for (int i = 0; i < num_iterations; ++i) {
		int number = rand() % 25;
		treap.insert(0, number);
		{
			std::lock_guard<std::mutex> lk{cout_lock};
			cout << "producer inserted " << number << '\n';
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	finished = true;
}

void consumer(Treap<int> &treap) {
	while (!finished || (treap.size() != 0)) {

		int sum = treap.sum();
		{
			std::lock_guard<std::mutex> lk{cout_lock};
			cout << "consumer has sum of " << sum << '\n';
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		treap.erase(0, 1);
	}
}


int main(void) {

	srand((unsigned)time(0));

	Treap<int> test = {1, 2, 4, 3};

	cout << test.sum() << endl;

	std::jthread thread1(consumer, std::ref(test));
	std::jthread thread2(producer, std::ref(test), 25);

	// test.mtx.lock();

	// for (auto it = test.begin(); it != test.end(); it++) {
	// 	cout << *it << '\n';
	// }

	// test.mtx.unlock();

	return 0;
}
