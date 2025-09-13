#pragma once
#include <cmath>
#include <iostream>

struct Pos3D;  // 前置声明，供 Pos2D 的方法引用

struct Pos2D {
    float x{0.f}, y{0.f};

    // --- 构造/转换 ---
    constexpr Pos2D() = default;
    constexpr Pos2D(float x_, float y_) : x(x_), y(y_) {}
    // 提升为 3D（z 默认 0）
    struct Pos3D with_z(float z = 0.f) const noexcept;

    // --- 一元 ---
    constexpr Pos2D operator+() const noexcept { return *this; }
    constexpr Pos2D operator-() const noexcept { return {-x, -y}; }

    // --- 复合赋值（2D）---
    constexpr Pos2D& operator+=(const Pos2D& r) noexcept {
        x += r.x;
        y += r.y;
        return *this;
    }
    constexpr Pos2D& operator-=(const Pos2D& r) noexcept {
        x -= r.x;
        y -= r.y;
        return *this;
    }
    constexpr Pos2D& operator*=(float s) noexcept {
        x *= s;
        y *= s;
        return *this;
    }
    constexpr Pos2D& operator/=(float s) noexcept {
        x /= s;
        y /= s;
        return *this;
    }

    // --- 二元（成员）---
    // 2D ± 2D
    constexpr Pos2D operator+(const Pos2D& r) const noexcept {
        Pos2D t = *this;
        t += r;
        return t;
    }
    constexpr Pos2D operator-(const Pos2D& r) const noexcept {
        Pos2D t = *this;
        t -= r;
        return t;
    }
    constexpr Pos2D operator*(float s) const noexcept {
        Pos2D t = *this;
        t *= s;
        return t;
    }
    constexpr Pos2D operator/(float s) const noexcept {
        Pos2D t = *this;
        t /= s;
        return t;
    }

    // 2D ± 3D → 3D（把自身当 z=0）
    struct Pos3D operator+(const struct Pos3D& r) const noexcept;
    struct Pos3D operator-(const struct Pos3D& r) const noexcept;

    // --- 几何 ---
    constexpr float sqr_length() const noexcept { return x * x + y * y; }
    float length() const noexcept { return std::sqrt(sqr_length()); }

    // 点积
    constexpr float dot(const Pos2D& r) const noexcept { return x * r.x + y * r.y; }
    float dot(const struct Pos3D& r) const noexcept;  // 忽略 r.z

    // 叉积：2D×2D → 标量(z)，2D×3D → 3D
    constexpr float cross(const Pos2D& r) const noexcept { return x * r.y - y * r.x; }
    struct Pos3D cross(const struct Pos3D& r) const noexcept;
};

struct Pos3D {
    float x{0.f}, y{0.f}, z{0.f};

    // --- 构造/转换 ---
    constexpr Pos3D() = default;
    constexpr Pos3D(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}
    // 从 2D 提升（z 可指定）
    constexpr explicit Pos3D(const Pos2D& v, float z_ = 0.f) : x(v.x), y(v.y), z(z_) {}
    // 降到 2D（丢 z）
    constexpr Pos2D xy() const noexcept { return Pos2D{x, y}; }

    // --- 一元 ---
    constexpr Pos3D operator+() const noexcept { return *this; }
    constexpr Pos3D operator-() const noexcept { return {-x, -y, -z}; }

    // --- 复合赋值（3D/2D）---
    constexpr Pos3D& operator+=(const Pos3D& r) noexcept {
        x += r.x;
        y += r.y;
        z += r.z;
        return *this;
    }
    constexpr Pos3D& operator-=(const Pos3D& r) noexcept {
        x -= r.x;
        y -= r.y;
        z -= r.z;
        return *this;
    }
    constexpr Pos3D& operator*=(float s) noexcept {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }
    constexpr Pos3D& operator/=(float s) noexcept {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
    // 与 2D
    constexpr Pos3D& operator+=(const Pos2D& r) noexcept {
        x += r.x;
        y += r.y;
        return *this;
    }
    constexpr Pos3D& operator-=(const Pos2D& r) noexcept {
        x -= r.x;
        y -= r.y;
        return *this;
    }

    // --- 二元（成员）---
    // 3D ± 3D
    constexpr Pos3D operator+(const Pos3D& r) const noexcept {
        Pos3D t = *this;
        t += r;
        return t;
    }
    constexpr Pos3D operator-(const Pos3D& r) const noexcept {
        Pos3D t = *this;
        t -= r;
        return t;
    }
    constexpr Pos3D operator*(float s) const noexcept {
        Pos3D t = *this;
        t *= s;
        return t;
    }
    constexpr Pos3D operator/(float s) const noexcept {
        Pos3D t = *this;
        t /= s;
        return t;
    }

    // 3D ± 2D → 3D
    constexpr Pos3D operator+(const Pos2D& r) const noexcept {
        Pos3D t = *this;
        t += r;
        return t;
    }
    constexpr Pos3D operator-(const Pos2D& r) const noexcept {
        Pos3D t = *this;
        t -= r;
        return t;
    }

    // --- 几何 ---
    constexpr float sqr_length() const noexcept { return x * x + y * y + z * z; }
    float length() const noexcept { return std::sqrt(sqr_length()); }

    // 点积
    constexpr float dot(const Pos3D& r) const noexcept { return x * r.x + y * r.y + z * r.z; }
    constexpr float dot(const Pos2D& r) const noexcept { return x * r.x + y * r.y; }  // 忽略自身 z

    // 叉积：3D×3D/2D → 3D
    constexpr Pos3D cross(const Pos3D& r) const noexcept {
        return Pos3D{y * r.z - z * r.y, z * r.x - x * r.z, x * r.y - y * r.x};
    }
    constexpr Pos3D cross(const Pos2D& r) const noexcept { return cross(Pos3D{r, 0.f}); }

    friend std::ostream& operator<<(std::ostream& os, const Pos3D& v) {
        os << "start: (" << v.x << v.y << v.z << ")";
        return os;
    }
};

// ====== Pos2D 里用到的对 Pos3D 的成员实现 ======
inline Pos3D Pos2D::with_z(float z) const noexcept { return Pos3D{x, y, z}; }

inline float Pos2D::dot(const Pos3D& r) const noexcept { return x * r.x + y * r.y; }  // 忽略 r.z

inline Pos3D Pos2D::operator+(const Pos3D& r) const noexcept {
    return Pos3D{x + r.x, y + r.y, r.z};
}
inline Pos3D Pos2D::operator-(const Pos3D& r) const noexcept {
    return Pos3D{x - r.x, y - r.y, -r.z};
}

inline Pos3D Pos2D::cross(const Pos3D& r) const noexcept {
    // (x,y,0) × (r.x,r.y,r.z)
    return Pos3D{y * r.z - 0.f * r.y, 0.f * r.x - x * r.z, x * r.y - y * r.x};
}
