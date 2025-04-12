#pragma once

#include "types/vec.hpp"

#include <array>

// A simple matrix class that stores elements in row-major order.
template <typename T, std::size_t Rows, std::size_t Cols> class Matrix {
public:
    Matrix() = default;

    // Construction via initializer list of Vecs - each Vec is a row
    template <typename... Args>
        requires(sizeof...(Args) == Rows) && (std::is_constructible_v<Args, Vec<T, Cols>> && ...)
    constexpr Matrix(Args... args) {
        static_assert(sizeof...(Args) == Rows, "Number of arguments must match the number of rows");
        std::array<Vec<T, Cols>, Rows> vecs{static_cast<Vec<T, Cols>>(args)...};
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < Cols; ++j) {
                m_data[index(i, j)] = vecs[i][j];
            }
        }
    }

    // Construction via initializer list of values - row-major order
    template <typename... Args>
        requires(sizeof...(Args) == Rows * Cols) && (std::is_constructible_v<Args, T> && ...)
    constexpr Matrix(Args... args) : m_data{static_cast<T>(args)...} {}

    constexpr static Matrix<T, Rows, Cols> identity()
        requires(Rows == Cols)
    {
        Matrix<T, Rows, Cols> result{};

        for (std::size_t i = 0; i < Rows; ++i) {
            result.at(i, i) = static_cast<T>(1);
        }

        return result;
    }

    // Operators
    template <std::size_t OtherRows, std::size_t OtherCols>
    constexpr Matrix<T, Rows, OtherCols> operator*(const Matrix<T, OtherRows, OtherCols>& other) const
        requires(Cols == OtherRows)
    {
        Matrix<T, Rows, OtherCols> result{};

        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < OtherCols; ++j) {
                auto row = this->row(i);
                auto col = other.col(j);
                result.at(i, j) = row.dot(col);
            }
        }
        return result;
    }

    constexpr Matrix<T, Cols, Rows> transpose() const {
        Matrix<T, Cols, Rows> result{};
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < Cols; ++j) {
                result.at(j, i) = at(i, j);
            }
        }
        return result;
    }

    constexpr Vec<T, Cols> row(std::size_t row) const {
        if (row >= Rows || row < 0) {
            throw std::out_of_range("Matrix index out of range");
        }

        Vec<T, Cols> result{};
        for (std::size_t i = 0; i < Cols; ++i) {
            result[i] = m_data[index(row, i)];
        }
        return result;
    }

    constexpr Vec<T, Rows> col(std::size_t col) const {
        if (col >= Cols || col < 0) {
            throw std::out_of_range("Matrix index out of range");
        }

        Vec<T, Rows> result{};
        for (std::size_t i = 0; i < Rows; ++i) {
            result[i] = m_data[index(i, col)];
        }
        return result;
    }

    // Const accessor
    constexpr const T& at(std::size_t row, std::size_t col) const {
        if (row >= Rows || col >= Cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[index(row, col)];
    }

    // Mutable accessor
    constexpr T& at(std::size_t row, std::size_t col) {
        if (row >= Rows || col >= Cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[index(row, col)];
    }

private:
    std::array<T, Rows * Cols> m_data{};

    constexpr inline std::size_t index(std::size_t row, std::size_t col) const { return row * Cols + col; }
};

using Matrix4x4f = Matrix<float, 4, 4>;