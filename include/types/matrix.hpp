#pragma once

#include "types/vec.hpp"

#include <array>

// A simple matrix class that stores elements in row-major order.
template <typename T, std::size_t Rows, std::size_t Cols> class Matrix {
public:
    Matrix() = default;

    Matrix(const std::array<Vec<T, Cols>, Rows>& data) : m_data(data) {}

    Matrix(const std::array<std::array<T, Cols>, Rows>& data) {
        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < Cols; ++j) {
                m_data[i][j] = data[i][j];
            }
        }
    }

    static Matrix<T, Rows, Cols> identity()
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
    Matrix<T, Rows, OtherCols> operator*(const Matrix<T, OtherRows, OtherCols>& other) const
        requires(Cols == OtherRows)
    {
        Matrix<T, Rows, OtherCols> result{};

        for (std::size_t i = 0; i < Rows; ++i) {
            for (std::size_t j = 0; j < OtherCols; ++j) {
                result.at(i, j) = at(i, 0) * other.at(0, j);
                for (std::size_t k = 1; k < Cols; ++k) {
                    result.at(i, j) += at(i, k) * other.at(k, j);
                }
            }
        }
        return result;
    }

    // Const accessors
    const T& at(std::size_t row, std::size_t col) const {
        if (row >= Rows || col >= Cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[row][col];
    }
    const Vec<T, Cols>& operator[](std::size_t index) const {
        if (index >= Rows) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[index];
    }

    // Mutable accessors
    T& at(std::size_t row, std::size_t col) {
        if (row >= Rows || col >= Cols) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[row][col];
    }
    Vec<T, Cols>& operator[](std::size_t index) {
        if (index >= Rows) {
            throw std::out_of_range("Matrix index out of range");
        }
        return m_data[index];
    }

private:
    std::array<Vec<T, Cols>, Rows> m_data{};
};

using Matrix4x4f = Matrix<float, 4, 4>;