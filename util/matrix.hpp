#ifndef UTIL_MATRIX_HPP
#define UTIL_MATRIX_HPP

#include <assert.h>
#include <array>
#include <iostream>

namespace util
{

template <typename T, int ROWS, int COLS>
class Matrix
{
public:
    using valueType = T;
    using Iterator = valueType*;
    using constIterator = const valueType*;
    using reference = valueType&;

    friend std::ostream& operator<<(std::ostream& out, const Matrix& mat)
    {
        for (int i = 0; i < ROWS; ++i)
        {
            if (i != 0)
            {
                out << std::endl;
            }
            for (int j = 0; j < COLS; ++j)
            {
                if (j != 0)
                {
                    out << " ";
                }
                out << mat[i][j];
            }
        }
        return out;
    }

    friend std::istream& operator>>(std::istream& in, Matrix& mat)
    {
        for (int i = 0; i < ROWS * COLS; ++i)
            in >> mat.data_[i];
        return in;
    }

    friend Matrix operator+(const Matrix& a, const Matrix& b)
    {
        Matrix res = a;
        res += b;
        return res;
    }

    template <typename Q, int m, int n, int p>
    friend Matrix<Q, m, p> operator*(const Matrix<Q, m, n>& a, const Matrix<Q, n, p>& b);

public:

    static Matrix eye()
    {
        assert(ROWS == COLS);
        Matrix res(0);
        for (int i = 0; i < ROWS; ++i)
            res[i][i] = 1;
        return res;
    }

    Matrix() = default;

    Matrix(std::initializer_list<T> data)
    { std::copy(data.begin(), data.end(), data_.begin()); }

    Matrix(const std::array<T, ROWS* COLS>& data): data_(data) {}

    Matrix(const T& element) { fill(element); }

    void fill(const T& element)
    { std::fill(data_.begin(), data_.end(), element); }


    struct Rect
    {
        Matrix* mat;
        std::pair<int, int> rowRange, colRange;

        Rect(Matrix* _mat, const std::pair<int, int>& _rowRange, const std::pair<int, int>& _colRange)
            : mat(_mat), rowRange(_rowRange), colRange(_colRange) {}

        Rect& operator=(const Rect& rect)
        {
            assert(rowRange.second - rowRange.first == rect.rowRange.second - rect.rowRange.first);
            assert(colRange.second - colRange.first == rect.colRange.second - rect.colRange.first);
            for (int i = rect.rowRange.first; i < rect.rowRange.second; ++i)
            {
                std::copy((*rect.mat)[i] + rect.colRange.first, (*rect.mat)[i] + rect.colRange.second,
                          (*mat)[i - rect.rowRange.first + rowRange.first] + colRange.first);
            }
            return *this;
        }


        template <int M_ROWS, int M_COLS>
        Rect& operator=(const Matrix<T, M_ROWS, M_COLS>& mat)
        {
            assert(rowRange.second - rowRange.first == M_ROWS);
            assert(colRange.second - colRange.first == M_COLS);
            for (int i = 0; i < mat.rows(); ++i)
            {
                std::copy(mat[i], mat[i + 1], (*this->mat)[i + rowRange.first] + colRange.first);
            }
            return *this;
        }
    };


    Matrix& operator+=(const Matrix& b)
    {
        auto pb = b.data_.begin();
        auto end = data_.end();

        for (T* p = data_.begin(); p != end; ++p)
        {
            *p += *pb++;
        }
        return *this;
    }


    Rect operator()(const std::pair<int, int>& rowRange,
                    const std::pair<int, int>& colRange)
    { return Rect(this, rowRange, colRange); }


    Matrix Transpose()
    {
        Matrix<T, COLS, ROWS> res;

        auto ptrDst = res.data_.begin();

        for (int i = 0; i != res.rows(); ++i)
        {
            auto ptrSrc = data_.begin() + i;
            for (int j = 0; j != res.cols(); ++j)
            {
                *ptrDst++ = *ptrSrc;
                ptrSrc += ROWS;
            }
        }
        return res;
    }

    Matrix invert();


    T* operator[](int i) { return data() + i * COLS; }
    const T* operator[](int i) const { return data() + i * COLS; }

    T* data() { return data_.begin(); }
    const T* data() const { return data_.cbegin(); }

    int rows() const { return ROWS; }
    int cols() const { return COLS; }

private:
    std::array<T, ROWS* COLS> data_;
};

template <typename Q, int m, int n, int p>
inline Matrix<Q, m, p> operator*(const Matrix<Q, m, n>& a, const Matrix<Q, n, p>& b)
{
    Matrix<Q, m, p> res(0);

    auto ptrDst = res.data_.begin();

    for (int i = 0; i != res.rows(); ++i)
    {
        int offs = i * a.cols();
        for (int j = 0; j != res.cols(); ++j, ++ptrDst)
        {
            auto ptrSrcA = a.data_.begin() + offs;
            auto ptrSrcB = b.data_.begin() + j;

            for (int k = 0; k != a.cols(); ++k)
            {
                *ptrDst += *ptrSrcA * *ptrSrcB;
                ++ptrSrcA;
                ptrSrcB += b.cols();
            }

        }
    }
    return res;
}


template <typename T, int ROWS, int COLS>
Matrix<T, ROWS, COLS> Matrix<T, ROWS, COLS>::invert()
{
    assert(ROWS == COLS);

    constexpr int N = ROWS;
    Matrix<T, N, 2 * N> a;

    int i, j, k;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < (2 * N); j++)
        {
            if (j < N)
                a[i][j] = (*this)[i][j];
            else if (j == N + i)
                a[i][j] = 1;
            else
                a[i][j] = 0;
        }
    }
    T* p[N];
    for (i = 0; i < N; i++)
    {
        p[i] = *(a + i);
    }

    for (i = 0; i < N; i++)
    {
        int r = i;
        //按列选主元
        for (k = i + 1; k < N; k++)
        {
            if (fabs(p[k][i]) > fabs(p[r][i]))
            {
                r = k;
            }
        }
        if (fabs(p[r][i]) <= 1e-7)
        {
            throw std::exception("invert");
        }
        //行交换
        if (r > i)
        {
            T* tmp = p[r];
            p[r] = p[i];
            p[i] = tmp;
        }
        //消元
        for (k = 0; k < N; k++)
        {
            if (k != i)
            {
                T radio = p[k][i] / p[i][i];
                for (j = i; j < (2 * N); j++)
                {
                    p[k][j] -= p[i][j] * radio;
                }
            }
        }
    }

    Matrix<T, N, N> dst;
    //左边化为单位阵
    for (i = 0; i < N; i++)
    {
        for (j = N; j < (2 * N); j++)
        {
            dst[i][j - N] = p[i][j] / p[i][i];
        }
    }
    return dst;
}


#define MAKE_TYPEDEFS(Type, TypeSuffix, Size)                     \
    using Matrix##Size##TypeSuffix = Matrix<Type, Size, Size>;


#define MAKE_TYPEDEFS_ALL_SIZES(Type, TypeSuffix) \
    MAKE_TYPEDEFS(Type, TypeSuffix, 2) \
    MAKE_TYPEDEFS(Type, TypeSuffix, 3) \
    MAKE_TYPEDEFS(Type, TypeSuffix, 4)

MAKE_TYPEDEFS_ALL_SIZES(int, i)
MAKE_TYPEDEFS_ALL_SIZES(float, f)
MAKE_TYPEDEFS_ALL_SIZES(double, d)


#undef MAKE_TYPEDEFS
#undef MAKE_TYPEDEFS_ALL_SIZES



#define MAKE_TYPEDEFS(Size)                     \
    template <typename T>                           \
    using Matrix##Size = Matrix<T, Size, Size>;


MAKE_TYPEDEFS(2)
MAKE_TYPEDEFS(3)
MAKE_TYPEDEFS(4)


#undef MAKE_TYPEDEFS

} // namespace util

#endif // UTIL_MATRIX_HPP
