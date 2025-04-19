#pragma once

#include <array>
#include <cstring>
#include <functional>

// A simple vector that containing N elements of type T.
template <typename T, std::size_t N> class Vec {
public:
    Vec() : m_data{} {};

    // Construction via initializer list
    template <typename... Args>
        requires(sizeof...(Args) == N) && (std::is_constructible_v<Args, T> && ...)
    constexpr Vec(Args... args) : m_data{static_cast<T>(args)...} {}

    template <std::size_t M> constexpr Vec(const Vec<T, M>& other) {
        // Copy the data from the other vector
        std::memcpy(&m_data[0], &other[0], std::min(N, M) * sizeof(T));
    }

    [[nodiscard]] Vec cross(const Vec& other) const
        requires(N == 3)
    {
        return Vec(
            {y() * other.z() - z() * other.y(), z() * other.x() - x() * other.z(), x() * other.y() - y() * other.x()});
    }

    [[nodiscard]] T dot(const Vec& other) const {
        T sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += m_data[i] * other.m_data[i];
        }
        return sum;
    }

    [[nodiscard]] T length_squared() const {
        T sum = 0;
        for (std::size_t i = 0; i < N; ++i) {
            sum += m_data[i] * m_data[i];
        }
        return sum;
    }

    [[nodiscard]] T length() const { return std::sqrt(length_squared()); }

    [[nodiscard]] Vec unit() const {
        auto len = length();
        if (len == 0) {
            throw std::runtime_error("Cannot normalize a zero-length vector");
        }
        return *this / len;
    }

    [[nodiscard]] Vec& operator+=(const Vec& other) {
        for (std::size_t i = 0; i < N; ++i) {
            m_data[i] += other.m_data[i];
        }
        return *this;
    }

    [[nodiscard]] Vec& operator-=(const Vec& other) {
        for (std::size_t i = 0; i < N; ++i) {
            m_data[i] -= other.m_data[i];
        }
        return *this;
    }

    [[nodiscard]] Vec& operator*=(T scalar) {
        for (std::size_t i = 0; i < N; ++i) {
            m_data[i] *= scalar;
        }
        return *this;
    }

    [[nodiscard]] Vec& operator/=(T scalar) {
        for (std::size_t i = 0; i < N; ++i) {
            m_data[i] /= scalar;
        }
        return *this;
    }

    [[nodiscard]] Vec operator+(const Vec& other) const { return Vec{*this} += other; };
    [[nodiscard]] Vec operator-(const Vec& other) const { return Vec{*this} -= other; };
    [[nodiscard]] Vec operator*(T scalar) const { return Vec{*this} *= scalar; };
    [[nodiscard]] Vec operator/(T scalar) const { return Vec{*this} /= scalar; };

    [[nodiscard]] bool operator==(const Vec& other) const {
        for (std::size_t i = 0; i < N; ++i) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }
        return true;
    }

    [[nodiscard]] bool operator!=(const Vec& other) const { return !(*this == other); }

    // Const accessors
    [[nodiscard]] const T& operator[](std::size_t index) const { return m_data[index]; }
    [[nodiscard]] const T& x() const
        requires(N >= 1)
    {
        return m_data[0];
    }
    [[nodiscard]] const T& y() const
        requires(N >= 2)
    {
        return m_data[1];
    }
    [[nodiscard]] const T& z() const
        requires(N >= 3)
    {
        return m_data[2];
    }
    [[nodiscard]] const T& w() const
        requires(N >= 4)
    {
        return m_data[3];
    }

    // Mutable accessors
    [[nodiscard]] T& operator[](std::size_t index) { return m_data[index]; }
    [[nodiscard]] T& x()
        requires(N >= 1)
    {
        return m_data[0];
    }
    [[nodiscard]] T& y()
        requires(N >= 2)
    {
        return m_data[1];
    }
    [[nodiscard]] T& z()
        requires(N >= 3)
    {
        return m_data[2];
    }
    [[nodiscard]] T& w()
        requires(N >= 4)
    {
        return m_data[3];
    }

    bool operator==(const Vec& other) {
        for (std::size_t i = 0; i < N; ++i) {
            if (m_data[i] != other.m_data[i]) {
                return false;
            }
        }
        return true;
    }

    const std::array<T, N>& data() const { return m_data; }

    [[nodiscard]] std::size_t size() const { return N; }

private:
    std::array<T, N> m_data;
};

using Vec2i = Vec<int, 2>;
using Vec2f = Vec<float, 2>;

using Vec3i = Vec<int, 3>;
using Vec3f = Vec<float, 3>;

using Vec4i = Vec<int, 4>;
using Vec4f = Vec<float, 4>;
