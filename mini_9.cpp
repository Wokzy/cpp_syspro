// #include <bits/stdc++.h>
#include <assert.h>
#include <utility>
#include <iostream>
#include <memory>


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

	shared_ptr<TreapNode> merge(shared_ptr<TreapNode> x, shared_ptr<TreapNode> y) {
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


	pair<shared_ptr<TreapNode>, shared_ptr<TreapNode>> splitBySize(shared_ptr<TreapNode> x, int k) {
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


	shared_ptr<TreapNode> insert(shared_ptr<TreapNode> x, int pos, T value) {
		auto l_r = splitBySize(x, pos - 1);
		auto y = make_shared<TreapNode>(rand(), value);
		return merge(merge(l_r.first, y), l_r.second);
	}

	shared_ptr<TreapNode> erase(shared_ptr<TreapNode> x, int pos, int count) {
		auto l_r = splitBySize(x, pos - 1);
		auto rl_rr = splitBySize(l_r.second, count);

		return merge(l_r.first, rl_rr.second);
	}


	shared_ptr<TreapNode> hp_node; // highest-priority node

public:

	Treap(): hp_node(nullptr) {}
	Treap(shared_ptr<TreapNode> node): hp_node(node) {}

	Treap(Treap &&other) {
		my_swap(hp_node, other.hp_node);
	}

	Treap(const Treap &other): hp_node(nullptr) {
		if (other.hp_node != nullptr) {
			hp_node = make_shared<TreapNode>(*other.hp_node);
		}
	}

	Treap &operator=(shared_ptr<TreapNode> node) {
		hp_node = node;
		return *this;
	}

	Treap &operator=(const Treap &other) {
		if (other.hp_node != nullptr) {
			hp_node = make_shared<TreapNode>(*other.hp_node);
		}

		return *this;
	}

	Treap &operator=(Treap &&other) {
		my_swap(hp_node, other.hp_node);

		return *this;
	}

	T sum(int lq, int rq) {
		auto l_r = splitBySize(hp_node, lq - 1);
		auto rl_rr = splitBySize(l_r.second, rq - lq + 1);

		T res = 0;
		if (rl_rr.first != nullptr) {
			res = rl_rr.first->sum;
		}

		merge(l_r.first, merge(rl_rr.first, rl_rr.second));
		return res;
	}

	void insert(int pos, T value) {
		hp_node = insert(hp_node, pos, value);
	}

	void erase(int pos, int count) {
		hp_node = erase(hp_node, pos, count);
	}
};


int main(void) {

	srand((unsigned)time(0));

	Treap<float> test{};
	test.insert(1, 1.1);
	test.insert(2, 14.4);
	test.insert(3, 25.7);
	test.insert(4, 25);
	test.insert(3, 10);

	cout << test.sum(1, 5) << '\n';

	return 0;
}
