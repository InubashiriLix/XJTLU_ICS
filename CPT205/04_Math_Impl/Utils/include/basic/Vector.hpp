#pragma once
#include <cmath>
#include <ostream>

#include "basic/Pos.hpp"

struct Vec3D {
    Pos3D s{0.f, 0.f, 0.f};  // start
    Pos3D e{0.f, 0.f, 0.f};  // end

    static constexpr float kEps = 1e-6f;  // epsilon

    /**
     *
     * @brief Constructs a Vec3D from two Pos3D points.
     *        Defaults both start and end to the origin if not specified.
     * @param start The starting position of the vector.
     * @param end The ending position of the vector.
     */
    explicit Vec3D(Pos3D start = Pos3D{0.f, 0.f, 0.f}, Pos3D end = Pos3D{0.f, 0.f, 0.f})
        : s(start), e(end) {}

    /**
     * @brief Constructs a Vec3D from six float values representing start and end coordinates.
     * @param s_x X coordinate of the start position.
     * @param s_y Y coordinate of the start position.
     * @param s_z Z coordinate of the start position.
     * @param e_x X coordinate of the end position.
     * @param e_y Y coordinate of the end position.
     * @param e_z Z coordinate of the end position.
     */
    explicit Vec3D(float s_x, float s_y, float s_z, float e_x, float e_y, float e_z)
        : s(Pos3D{s_x, s_y, s_z}), e(Pos3D{e_x, e_y, e_z}) {}

    /**
     * @brief Constructs a Vec3D from end coordinates, with start defaulting to the origin.
     * @param e_x X coordinate of the end position.
     * @param e_y Y coordinate of the end position.
     * @param e_z Z coordinate of the end position.
     */
    explicit Vec3D(float e_x, float e_y, float e_z)
        : s(Pos3D{0.f, 0.f, 0.f}), e(Pos3D{e_x, e_y, e_z}) {}

    /**
     * @brief Calculates the length (magnitude) of the vector.
     * @return The Euclidean distance between the start and end positions.
     */
    float getLen() const { return std::hypot(std::hypot(e.x - s.x, e.y - s.y), e.z - s.z); }

    /**
     * @brief Checks if the vector length is effectively zero.
     * @return True if the length is less than or equal to kEps, otherwise false.
     */
    bool isZeroLen() const { return getLen() <= kEps; }

    /**
     * @brief Checks if both the start and end positions are at the origin.
     * @return True if all coordinates of start and end are near zero, otherwise false.
     */
    bool isOriginal() const {
        auto near0 = [](float v) { return std::fabs(v) <= kEps; };
        return near0(s.x) && near0(s.y) && near0(s.z) && near0(e.x) && near0(e.y) && near0(e.z);
    }

    /**
     * @brief Gets the position vector from the origin to the vector's displacement.
     * @return A Vec3D representing the displacement from the origin.
     */
    Vec3D getPositionVec() const {
        Pos3D d = e - s;
        return Vec3D(Pos3D{0.f, 0.f, 0.f}, d);
    }

    /**
     * @brief Gets the unit (normalized) vector in the direction of the vector.
     * @return A Vec3D representing the unit vector, or zero vector if length is zero.
     */
    Vec3D getUnit() const {
        float L = getLen();
        if (L <= kEps) return Vec3D(0.f, 0.f, 0.f);
        Pos3D d = (e - s) / L;
        return Vec3D(Pos3D{0.f, 0.f, 0.f}, d);
    }

    Vec3D operator+(const Vec3D& other) const {
        Pos3D d = other.getPositionVec().e;
        return Vec3D(this->s, this->e + d);
    }

    Vec3D operator-(const Vec3D& other) const {
        Pos3D d = other.getPositionVec().e;
        return Vec3D(this->s, this->e - d);
    }

    Vec3D& operator+=(const Vec3D& other) {
        this->e += other.getPositionVec().e;
        return *this;
    }

    Vec3D& operator-=(const Vec3D& other) {
        this->e -= other.getPositionVec().e;
        return *this;
    }

    Vec3D& operator*=(float scaler) {
        Pos3D d = (this->e - this->s) * scaler;
        this->e = this->s + d;
        return *this;
    }

    Vec3D& operator/=(float denom) {
        float inv = 1.0f / denom;
        Pos3D d = (this->e - this->s) * inv;
        this->e = this->s + d;
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec3D& v) {
        os << "start: " << v.s << " end: " << v.e;
        return os;
    }
};

struct Vec2D : Vec3D {
    /**
     * @brief Constructs a Vec2D from two Pos2D points.
     *        Defaults both start and end to the origin if not specified.
     * @param start The starting position of the vector (2D).
     * @param end The ending position of the vector (2D).
     */
    explicit Vec2D(Pos2D start = Pos2D{0.f, 0.f}, Pos2D end = Pos2D{0.f, 0.f})
        : Vec3D(Pos3D{start.x, start.y, 0.f}, Pos3D{end.x, end.y, 0.f}) {}

    /**
     * @brief Constructs a Vec2D from four float values representing start and end coordinates.
     * @param s_x X coordinate of the start position.
     * @param s_y Y coordinate of the start position.
     * @param e_x X coordinate of the end position.
     * @param e_y Y coordinate of the end position.
     */
    explicit Vec2D(float s_x, float s_y, float e_x, float e_y)
        : Vec3D(Pos3D{s_x, s_y, 0.f}, Pos3D{e_x, e_y, 0.f}) {}

    /**
     * @brief Constructs a Vec2D from end coordinates, with start defaulting to the origin.
     * @param e_x X coordinate of the end position.
     * @param e_y Y coordinate of the end position.
     */
    explicit Vec2D(float e_x, float e_y) : Vec3D(Pos3D{0.f, 0.f, 0.f}, Pos3D{e_x, e_y, 0.f}) {}
};
