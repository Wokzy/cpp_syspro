
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <vector>

typedef double fp64;

using namespace std;

class SquareMatrix {
    fp64* data; // always row-major

    struct SquareMatrixView {
    private:
        fp64* row;
    public:

        SquareMatrixView(fp64 *ptr) : row(ptr) {}

        fp64 &operator[](size_t index) const {
            return row[index];
        }

        ~SquareMatrixView() {};
    };

public:

    const size_t n;

    SquareMatrix(size_t n) : n(n) {
        data = new fp64[n * n];
        zeros();
    }

    SquareMatrix(vector<fp64> diag) : n(diag.size()) {
        assert(n > 0);

        data = new fp64[n * n];
        zeros();

        for (size_t i = 0; i < n; ++i) {
            data[i * n + i] = diag[i];
        }
    }

    SquareMatrix(const SquareMatrix &other) : n(other.n) {
        data = new fp64[n * n];
        memcpy(data, other.data, sizeof(fp64) * n * n);
    }

    SquareMatrix(SquareMatrix &&other) : n(other.n), data(other.data) {
        other.data = nullptr;
    }

    SquareMatrix &operator=(const SquareMatrix &other) {
        assert(other.n == n);

        delete[] data;
        data = new fp64[n * n];
        memcpy(data, other.data, sizeof(fp64) * n * n);
    }

    SquareMatrix &operator=(SquareMatrix &&other) {
        assert(other.n == n);
        fp64* tmp = other.data;
        other.data = data;
        data = tmp;

        return *this;
    }

    ~SquareMatrix() {
        delete[] data;
    }

    SquareMatrixView operator[](size_t row) const {
        return SquareMatrixView(data + row * n);
    }

    void zeros() {
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                data[i * n + j] = 0;
    }

    void ones() {
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                data[i * n + j] = 1;
    }

    explicit operator fp64() const {
        fp64 sum = 0.0;

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                sum += data[i * n + j];

        return sum;
    }

    friend SquareMatrix operator+(const SquareMatrix& first, const SquareMatrix& second) {
        // just to demo copy constructor usage!!!
        SquareMatrix res = first;

        return res += second;
    }

    friend SquareMatrix operator*(const SquareMatrix& first, const SquareMatrix& second) {
        size_t n = first.n;
        assert(n == second.n);

        SquareMatrix tmp = SquareMatrix(n);

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                for (size_t k = 0; k < n; k++)
                    tmp[i][j] += first.data[i * n + k] * second.data[k * n + j];

        return tmp;
    }

    friend SquareMatrix operator*(const fp64 scalar, const SquareMatrix& second) {
        size_t n = second.n;
        assert(n == second.n);

        SquareMatrix tmp = SquareMatrix(n);

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                tmp[i][j] += scalar * second[i][j];

        return tmp;
    }

    SquareMatrix &operator+=(const SquareMatrix& other) {
        assert(n == other.n);

        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                data[i * n + j] += other.data[i * n + j];

        return *this;
    }

    SquareMatrix &operator*=(const SquareMatrix& other) {
        // just to demo move constructor usage!!!
        return *this = (*this) * other;
    }

    SquareMatrix &operator*=(const fp64 scalar) {
        return *this = scalar * (*this);
    }

    bool operator==(const SquareMatrix& other) {
        for (size_t i = 0; i < n; ++i)
            for (size_t j = 0; j < n; ++j)
                if (data[i * n + j] != other.data[i * n + j]) return false;

        return true;
    }

    bool operator!=(const SquareMatrix &other) {
        return !((*this) == other);
    }

    friend ostream& operator<<(ostream& os, const SquareMatrix& matrix) {

        for (size_t i = 0; i < matrix.n; i++) {
            os << "[";
            for (size_t j = 0; j < matrix.n; j++) {
                os << matrix[i][j];
                if (j != matrix.n - 1) os << ", ";
            }
            os << "]\n";
        }

        return os;
    }
};

signed main() {
    SquareMatrix matrix = SquareMatrix(vector<fp64>(4, 4.0));
    matrix[1][0] = 5.0;

    // matrix *= matrix;
    matrix *= 3.0;

    // cout << matrix << endl;

    // cout << matrix * matrix << endl;
    cout << matrix << endl;
    cout << (matrix != matrix) << endl;
    cout << (fp64) matrix << endl;
}
