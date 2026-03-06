// #include <bits/stdc++.h>
#include <assert.h>
#include <utility>
#include <iostream>
#include <memory>


using namespace std;

struct Treap {
	int priority;
	int sum;
	int size;
	int value;
	shared_ptr<Treap> right;
	shared_ptr<Treap> left;

	Treap(int p, int val) : sum(val), value(val), priority(p), size(1), right(nullptr), left(nullptr) {}
	Treap(const Treap &other) : priority(other.priority), sum(other.sum), size(other.size),
								value(other.value) {

		if (other.right != nullptr) {
			right = make_shared<Treap>(*other.right);
		}

		if (other.left != nullptr) {
			left = make_shared<Treap>(*other.left);
		}
	}

	Treap(Treap &&other) : priority(other.priority), sum(other.sum), size(other.size),
						value(other.value), right(other.right), left(other.left) {
		cout << "move ctr called\n";
	}

	Treap &operator=(const Treap &other) {
		priority = other.priority;
		sum = other.sum;
		size = other.size;
		value = other.value;

		if (other.right != nullptr) {
			right = make_shared<Treap>(*other.right);
		}

		if (other.left != nullptr) {
			left = make_shared<Treap>(*other.left);
		}

		return *this;
	}

	Treap &operator=(Treap &&other) {
		priority = other.priority;
		sum = other.sum;
		size = other.size;
		value = other.value;
		right = other.right;
		left = other.left;

		cout << "move assign called\n";

		return *this;
	}
};



void update(shared_ptr<Treap> x) {
	x->size = 1;
	x->sum = x->value;
	if (x->left) {
		x->size += x->left->size;
		x->sum += x->left->sum;
	}
	if (x->right) {
		x->size += x->right->size;
		x->sum += x->right->sum;
	}
}


shared_ptr<Treap> merge(shared_ptr<Treap> x, shared_ptr<Treap> y) {
	if (x == nullptr) return y;
	if (y == nullptr) return x;

	if (x->priority < y->priority) {
		x->right = merge(x->right, y);
		update(x->right);
		update(x);
		return x;
	}

	y->left = merge(x, y->left);
	update(y->left);
	update(y);
	return y;
}


pair<shared_ptr<Treap> , shared_ptr<Treap> > splitBySize(shared_ptr<Treap> x, int k) {
	if (x == nullptr) return {nullptr, nullptr};

	int l_size = 0;
	if (x->left) l_size = x->left->size;

	if (k <= l_size) {
		auto ll_lr = splitBySize(x->left, k);
		x->left = ll_lr.second;
		update(x);
		return {ll_lr.first, x};
	}

	auto rl_rr = splitBySize(x->right, k - l_size - 1);
	x->right = rl_rr.first;
	update(x);
	return {x, rl_rr.second};
}


shared_ptr<Treap> insert(shared_ptr<Treap> x, int pos, int value) {
	auto l_r = splitBySize(x, pos - 1);
	auto y = make_shared<Treap>(rand(), value);
	return merge(merge(l_r.first, y), l_r.second);
}

shared_ptr<Treap> erase(shared_ptr<Treap> x, int pos, int count) {
	auto l_r = splitBySize(x, pos - 1);
	auto rl_rr = splitBySize(l_r.second, count);

	return merge(l_r.first, rl_rr.second);
}

int sum(shared_ptr<Treap> x, int lq, int rq) {
	auto l_r = splitBySize(x, lq - 1);
	auto rl_rr = splitBySize(l_r.second, rq - lq + 1);

	int res = 0;
	if (rl_rr.first != nullptr) {
		res = rl_rr.first->sum;
	}

	merge(l_r.first, merge(rl_rr.first, rl_rr.second));
	return res;
}


void test_1(shared_ptr<Treap> treap) {
	assert(sum(treap, 1, 1) == 1);
	assert(sum(treap, 1, 10) == 75);
	assert(sum(treap, 3, 4) == 35);
	assert(sum(treap, 100, 230) == 0);
	assert(sum(treap, 4, 4) == 25);


	treap = erase(treap, 3, 2);
	// {1, 14, 25};
	assert(sum(treap, 1, 10) == 40);
	assert(sum(treap, 2, 2) == 14);
}

void test_2(shared_ptr<Treap> treap) {
	assert(sum(treap, 1, 1) == 1);
	assert(sum(treap, 1, 10) == 55);
	assert(sum(treap, 3, 4) == 15);
	assert(sum(treap, 100, 230) == 0);
	assert(sum(treap, 4, 4) == 5);


	treap = erase(treap, 3, 2);
	// {1, 14, 25};
	assert(sum(treap, 1, 10) == 40);
	assert(sum(treap, 2, 2) == 14);
}

template<typename T>
void my_swap(T& a, T& b) {
	T tmp = move(a);
	a = move(b);
	b = move(tmp);
}


int main(void) {

	srand((unsigned)time(0));

	auto test = insert(nullptr, 1, 1);
	test = insert(test, 2, 14);
	test = insert(test, 3, 25);
	test = insert(test, 4, 25);
	test = insert(test, 3, 10);

	auto test_moved = insert(nullptr, 1, 1);
	test_moved = insert(test_moved, 2, 14);
	test_moved = insert(test_moved, 3, 5);
	test_moved = insert(test_moved, 4, 25);
	test_moved = insert(test_moved, 3, 10);

	my_swap(*test, *test_moved);


	// {1, 14, 10, 25, 25}


	test_1(test_moved);
	test_2(test);

	return 0;
}
