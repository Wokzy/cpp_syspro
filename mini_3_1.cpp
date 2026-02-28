
#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>

using namespace std;


class PersistentQueue {
private:

	struct Node {
		shared_ptr<Node> next = nullptr;
		shared_ptr<Node> element = nullptr;

		int value;

		Node() : next(nullptr), value(0), element(nullptr) {}
		Node(shared_ptr<Node> next, int value) : next(next), value(value), element(nullptr) {}
		Node(shared_ptr<Node> next, int value, shared_ptr<Node> element) : next(next), value(value), element(element) {}
		Node(shared_ptr<Node> next, shared_ptr<Node> element) : next(next), value(0), element(element) {}

		Node(const Node &other) : value(other.value) {
			// cout << "Copy node\n";
			if (other.next != nullptr) {
				next = make_shared<Node>(*other.next);
			}
			if (other.element != nullptr) {
				element = make_shared<Node>(*other.element);
			}
		}
	};

	struct VersionInfo {
		int size;
		int ready_list_size = 0;
		int building_list_size = 0;
		shared_ptr<Node> head = nullptr;
		shared_ptr<Node> tail = nullptr;
		shared_ptr<Node> ready_list = nullptr;
		shared_ptr<Node> building_list = nullptr;

		VersionInfo(int size, shared_ptr<Node> tail, shared_ptr<Node> head) : size(size), head(head), tail(tail), ready_list(nullptr), building_list(nullptr) {}
		VersionInfo(int size, shared_ptr<Node> tail, shared_ptr<Node> head, shared_ptr<Node> ready_list, shared_ptr<Node> building_list) : size(size), head(head), tail(tail), ready_list(ready_list), building_list(building_list) {}

		VersionInfo(const VersionInfo &other) : size(other.size),
												ready_list_size(other.ready_list_size),
												building_list_size(other.building_list_size) {
			
			cout << "version copy\n";
			if (other.head != nullptr) {
				head = make_shared<Node>(*other.head);
			}
			if (other.tail != nullptr) {
				tail = make_shared<Node>(*other.tail);
			}
			if (other.ready_list != nullptr) {
				ready_list = make_shared<Node>(*other.ready_list);
			}
			if (other.building_list != nullptr) {
				building_list = make_shared<Node>(*other.building_list);
			}
		}

		bool _check_ready_list() {
			if (building_list->element->next == head) {
				ready_list = building_list;
				ready_list_size = building_list_size;
				building_list = nullptr;
				building_list_size = 0;
				return true;
			}
			return false;
		}

		void maybe_build_list() {

			if (building_list == nullptr) {
				if (2 * ready_list_size >= (size - 2)) return;

				building_list = make_shared<Node>(nullptr, 0, tail->next);
				building_list_size++;
				_check_ready_list();
			} else if (!_check_ready_list()) {
				building_list = make_shared<Node>(building_list, 0, building_list->element->next);
				building_list_size++;
			}
		}
	};

	vector<shared_ptr<VersionInfo>> versions;


public:

	PersistentQueue() : versions(vector<shared_ptr<VersionInfo>>()) {
		versions.push_back(make_shared<VersionInfo>(0, nullptr, nullptr));
	}

	PersistentQueue(const PersistentQueue &other) {
		cout << "creating from other " << versions.size() << '\n';
		// versions.clear();
		for (auto ver : other.versions) {
			versions.push_back(make_shared<VersionInfo>(*ver));
		}
	}

	PersistentQueue &operator=(const PersistentQueue &other) {
		cout << "persisnent queue assign\n";
		versions.clear();
		for (auto ver : other.versions) {
			versions.push_back(make_shared<VersionInfo>(*ver));
		}

		return *this;
	}

	void push(int version, int value) {
		auto old_ = versions[version];

		auto new_ = make_shared<VersionInfo>(old_->size + 1, make_shared<Node>(old_->tail, value, nullptr), old_->head, old_->ready_list, old_->building_list);
		if (old_->size == 0) {
			new_->head = new_->tail;
		}

		new_->ready_list_size = old_->ready_list_size;
		new_->building_list_size = old_->building_list_size;
		new_->maybe_build_list();

		versions.push_back(new_);
	}

	int front(int version) {
		auto ver = versions[version];
		if (ver->size == 0) {
			throw runtime_error("front for empty queue");
		}

		return ver->head->value;
	}

	int pop(int version) {
		auto old_ = versions[version];

		if (old_->size == 0) {
			throw runtime_error("Pop for empty queue");
		} else if (old_->size == 1) {
			auto new_ = make_shared<VersionInfo>(0, nullptr, nullptr);
			versions.push_back(new_);
			return old_->tail->value;
		} else if (old_->size == 2) {
			auto new_ = make_shared<VersionInfo>(1, old_->tail, nullptr);
			versions.push_back(new_);
			return old_->head->value;
		}

		auto new_ = make_shared<VersionInfo>(old_->size - 1, old_->tail, old_->ready_list->element, old_->ready_list->next, old_->building_list);
		new_->ready_list_size = old_->ready_list_size - 1;
		new_->building_list_size = old_->building_list_size;
		new_->maybe_build_list();

		versions.push_back(new_);
		return old_->head->value;
	}
};


void test_copy() {
	auto queue = PersistentQueue();
	queue.push(0, 15);
	queue.push(1, 10);
	queue.push(2, 9999);

	// PersistentQueue copied = queue;
	PersistentQueue copied = PersistentQueue{};
	copied = queue;
	cout << "-------------- pop from original ---------------\n";

	cout << queue.pop(3) << endl;
	cout << queue.pop(4) << endl;
	cout << queue.pop(5) << endl;

	cout << "-------------- pop from copy ---------------\n";
	cout << copied.pop(2) << endl;
	cout << copied.pop(3) << endl;
	cout << copied.pop(4) << endl;
}


signed main() {

	test_copy();

	return 0;

	// auto queue = PersistentQueue();
	// queue.push(0, 15);
	// queue.push(1, 10);
	// queue.push(2, 25);
	// queue.push(3, 30);
	// queue.push(4, 45);
	// // queue.push(5, 50);
	// queue.push(2, 4); // -> 6
	// queue.push(6, 7); // -> 7
	// queue.push(6, -3); // -> 8
	// queue.push(7, 2); // -> 9
	// queue.push(9, 10); // -> 10

	// // cout << queue.pop(6) << '\n';
	// // cout << queue.pop(7) << '\n';
	// // cout << queue.pop(8) << '\n';
	// // cout << queue.pop(9) << '\n';
	// // cout << queue.pop(10) << '\n';

	// PersistentQueue copied = queue;

	// cout << "-------------- pop from original ---------------\n";

	// cout << queue.pop(10) << '\n';
	// cout << queue.pop(11) << '\n';
	// cout << queue.pop(12) << '\n';
	// cout << queue.pop(13) << '\n';
	// cout << queue.pop(14) << '\n';
	// cout << queue.pop(15) << '\n';
	// // cout << queue.pop(16) << '\n';

	// // cout << "----------------------\n";

	// cout << queue.pop(8) << '\n';
	// cout << queue.pop(17) << '\n';
	// cout << queue.pop(18) << '\n';
	// cout << queue.pop(19) << '\n';

	// cout << "-------------- pop from copy ---------------\n";

	// cout << copied.pop(10) << '\n';
	// cout << copied.pop(11) << '\n';
	// cout << copied.pop(12) << '\n';
	// cout << copied.pop(13) << '\n';
	// cout << copied.pop(14) << '\n';
	// cout << copied.pop(15) << '\n';

	// cout << copied.pop(8) << '\n';
	// cout << copied.pop(17) << '\n';
	// cout << copied.pop(18) << '\n';
	// cout << copied.pop(19) << '\n';


	// return 0;
}

