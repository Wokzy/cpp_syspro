#include <iostream>
#include <thread>
#include <mutex>
#include <stdexcept>
#include <assert.h>

// #define DEBUG

template <typename T>
class my_shared_ptr {
private:

	using element_type = std::conditional_t<std::is_array_v<T>, std::remove_extent_t<T>, T>;

	template<typename U> struct default_deleter {
		void operator() (U* ptr) {
			delete ptr;
		}
	};

	template<typename U> struct default_deleter<U[]> {
		void operator() (U* ptr) {
			delete[] ptr;
		}
	};

	template<typename Deleter = default_deleter<T>>
	struct control_block {
		element_type *obj_ptr = nullptr;
		Deleter obj_deleter;
		std::mutex mtx;

		size_t ref_counter = 1;

		template<typename... Args>
		control_block(Args... args) {
			obj_ptr = new element_type(args...);
		}

		control_block(element_type *ptr) : obj_ptr(ptr) {}
		control_block(nullptr_t ptr) : obj_ptr(nullptr) {}
		control_block() : obj_ptr(nullptr) {}

		~control_block() {
			if (obj_ptr != nullptr) {
				obj_deleter(obj_ptr);
			}
		}

		void inc_ref() {
			std::lock_guard<std::mutex> lk{mtx};
			++ref_counter;

			#ifdef DEBUG
			std::cout << "ref counter changed to " << ref_counter << std::endl;
			#endif
		}

		bool dec_ref() {
			std::lock_guard<std::mutex> lk{mtx};

			assert(ref_counter > 0);
			--ref_counter;

			#ifdef DEBUG
			std::cout << "ref counter changed to " << ref_counter << std::endl;
			#endif

			return (ref_counter == 0);
		}
	};

	control_block<> *cb = nullptr;

	void dec_ref() {
		if (cb != nullptr) {
			if (cb->dec_ref()) {
				delete cb;
				cb = nullptr;
			}
		}
	}

	void inc_ref() {
		if (cb != nullptr) {
			cb->inc_ref();
		}
	}

public:

	template<typename... Args>
	explicit my_shared_ptr(Args... args) : cb(new control_block(args...)) {}

	my_shared_ptr(element_type* p) : cb(new control_block(p)) {}
	my_shared_ptr(std::nullptr_t p) : cb(nullptr) {}
	my_shared_ptr() : cb(nullptr) {}

	my_shared_ptr(const my_shared_ptr &other) : cb(other.cb) {
		inc_ref();
	}

	my_shared_ptr(my_shared_ptr &&other) {
		std::swap(cb, other.cb);
	}

	my_shared_ptr& operator=(const my_shared_ptr &other) {
		if (cb == other.cb) {
			return *this;
		}

		dec_ref(); // relase older ownership

		cb = other.cb;
		inc_ref(); // add new owner
		return *this;
	}

	my_shared_ptr& operator=(my_shared_ptr &&other) {
		std::swap(cb, other.cb); // r-value "other" my_shared_ptr will dec_ref in destructor for us
		return *this;
	}

	~my_shared_ptr() {
		if (cb != nullptr) {
			dec_ref();
		}
	}

	element_type &operator*() const {
		return *cb->obj_ptr;
	}

	element_type &operator[](int idx) const {
		return *(cb->obj_ptr + idx);
	}

	element_type *operator->() const {
		return cb->obj_ptr;
	}

	bool friend operator==(const my_shared_ptr &first, const my_shared_ptr &second) {
		return first.cb == second.cb;
	}

	bool friend operator!=(const my_shared_ptr &first, const my_shared_ptr &second) {
		return first.cb != second.cb;
	}

	operator bool() const {

		if (cb != nullptr) {
			if (cb->obj_ptr != nullptr) return true;
		}

		return false;
	}
};


template<typename T, typename... Args>
inline my_shared_ptr<T> my_make_shared(Args... args) {
	return my_shared_ptr<T>(args...);
}


using namespace std;

template<typename T>
void my_swap(T& a, T& b) {
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}

class Treap {
public:
	struct TreapNode {
		int priority;
		int sum;
		int size;
		int value;
		my_shared_ptr<TreapNode> right;
		my_shared_ptr<TreapNode> left;

		TreapNode(int p, int val) : sum(val), value(val), priority(p), size(1), right(nullptr), left(nullptr) {}
		TreapNode(const TreapNode &other) : priority(other.priority), sum(other.sum), size(other.size),
									value(other.value) {

			if (other.right != nullptr) {
				right = my_make_shared<TreapNode>(*other.right);
			}

			if (other.left != nullptr) {
				left = my_make_shared<TreapNode>(*other.left);
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
				right = my_make_shared<TreapNode>(*other.right);
			}

			if (other.left != nullptr) {
				left = my_make_shared<TreapNode>(*other.left);
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

	my_shared_ptr<TreapNode> merge(my_shared_ptr<TreapNode> x, my_shared_ptr<TreapNode> y) {
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


	pair<my_shared_ptr<TreapNode>, my_shared_ptr<TreapNode>> splitBySize(my_shared_ptr<TreapNode> x, int k) {
		if (x == nullptr) return {nullptr, nullptr};

		int l_size = 0;
		if (x->left) {
			l_size = x->left->size;
		}

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


	my_shared_ptr<TreapNode> insert(my_shared_ptr<TreapNode> x, int pos, int value) {
		auto l_r = splitBySize(x, pos - 1);
		auto y = my_make_shared<TreapNode>(rand(), value);
		return merge(merge(l_r.first, y), l_r.second);
	}

	my_shared_ptr<TreapNode> erase(my_shared_ptr<TreapNode> x, int pos, int count) {
		auto l_r = splitBySize(x, pos - 1);
		auto rl_rr = splitBySize(l_r.second, count);

		return merge(l_r.first, rl_rr.second);
	}


	my_shared_ptr<TreapNode> hp_node; // highest-priority node

public:

	Treap(): hp_node(nullptr) {}
	Treap(my_shared_ptr<TreapNode> node): hp_node(node) {}

	Treap(Treap &&other) {
		my_swap(hp_node, other.hp_node);
	}

	Treap(const Treap &other): hp_node(nullptr) {
		if (other.hp_node != nullptr) {
			hp_node = my_make_shared<TreapNode>(*other.hp_node);
		}
	}

	Treap &operator=(my_shared_ptr<TreapNode> node) {
		hp_node = node;
		return *this;
	}

	Treap &operator=(const Treap &other) {
		if (other.hp_node != nullptr) {
			hp_node = my_make_shared<TreapNode>(*other.hp_node);
		}

		return *this;
	}

	Treap &operator=(Treap &&other) {
		my_swap(hp_node, other.hp_node);

		return *this;
	}

	int sum(int lq, int rq) {
		auto l_r = splitBySize(hp_node, lq - 1);
		auto rl_rr = splitBySize(l_r.second, rq - lq + 1);

		int res = 0;
		if (rl_rr.first != nullptr) {
			res = rl_rr.first->sum;
		}

		merge(l_r.first, merge(rl_rr.first, rl_rr.second));
		return res;
	}

	void insert(int pos, int value) {
		hp_node = insert(hp_node, pos, value);
	}

	void erase(int pos, int count) {
		hp_node = erase(hp_node, pos, count);
	}
};


void test_1(Treap &treap) {
	assert(treap.sum(1, 1) == 1);
	assert(treap.sum(1, 10) == 75);
	assert(treap.sum(3, 4) == 35);
	assert(treap.sum(100, 230) == 0);
	assert(treap.sum(4, 4) == 25);


	treap.erase(3, 2);
	// {1, 14, 25};
	assert(treap.sum(1, 10) == 40);
	assert(treap.sum(2, 2) == 14);
}

void test_2(Treap &treap) {
	assert(treap.sum(1, 1) == 1);
	assert(treap.sum(1, 10) == 55);
	assert(treap.sum(3, 4) == 15);
	assert(treap.sum(100, 230) == 0);
	assert(treap.sum(4, 4) == 5);


	treap.erase(3, 2);
	// {1, 14, 25};
	assert(treap.sum(1, 10) == 40);
	assert(treap.sum(2, 2) == 14);
}

std::atomic<bool> finished = false;
std::mutex cout_lock;

void producer(my_shared_ptr<int[]> ptr, int arr_size, int num_iterations) {
	for (int i = 0; i < num_iterations; ++i) {
		int number = rand() % 25;
		ptr[i % arr_size] = number;
		{
			std::lock_guard<std::mutex> lk{cout_lock};
			cout << "producer inserted " << number << '\n';
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	finished = true;
}

void consumer(my_shared_ptr<int[]> ptr, int arr_size) {
	while (!finished) {

		{
			std::lock_guard<std::mutex> lk{cout_lock};

			cout << "consumer has [ ";
			for (int i = 0; i < arr_size; ++i) {
				cout << ptr[i] << " ";
			}
			cout << "]\n";
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(125));
	}
}


int main(void) {

	srand((unsigned)time(0));

	Treap test{};
	test.insert(1, 1);
	test.insert(2, 14);
	test.insert(3, 25);
	test.insert(4, 25);
	test.insert(3, 10);

	Treap test_moved{};
	test_moved.insert(1, 1);
	test_moved.insert(2, 14);
	test_moved.insert(3, 5);
	test_moved.insert(4, 25);
	test_moved.insert(3, 10);

	my_swap(test, test_moved);

	// {1, 14, 10, 25, 25}

	test_1(test_moved);
	test_2(test);


	my_shared_ptr<int[]> some_arr = new int[8];

	std::thread thread1(consumer, some_arr, 8);
	std::thread thread2(producer, some_arr, 8, 25);

	thread1.join();
	thread2.join();

	cout << "main thread has [ ";
	for (int i = 0; i < 8; ++i) {
		cout << some_arr[i] << " ";
	}
	cout << "]\n";

	return 0;
}

