#pragma once

#include <GL/freeglut.h>
#include <GL/glut.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <tuple>
#include <utility>

#include "basic/Color.hpp"
#include "basic/Pos.hpp"

class Triangle2D {
   public:
    using triangleVertices_t = std::tuple<Pos2D, Pos2D, Pos2D>;
    using windows2dSize_t = std::pair<float, float>;

   public:
    Triangle2D(triangleVertices_t vertices, RGBA rgba) : m_vertices_base(vertices), m_rgba(rgba) {}

    triangleVertices_t getVertices() const { return transformedVerticesTuple(); }  // transformed
    void setVertices(triangleVertices_t vertices) { m_vertices_base = vertices; }

    void setColor(RGBA rgba) { m_rgba = rgba; }
    RGBA getColor() const { return m_rgba; }

    // scale about centroid (separate on x/y)
    void setScale(float sx, float sy) {
        m_scale_x = (sx <= 0.f ? 1.f : sx);
        m_scale_y = (sy <= 0.f ? 1.f : sy);
    }

    void setUniformScale(float s) { setScale(s, s); }

    std::pair<float, float> getScale() const { return {m_scale_x, m_scale_y}; }

    // translate by dx, dy
    void translate(float dx, float dy) {
        m_translate_x += dx;
        m_translate_y += dy;
    }
    void setTranslate(float tx, float ty) {
        m_translate_x = tx;
        m_translate_y = ty;
    }
    std::pair<float, float> getTranslate() const { return {m_translate_x, m_translate_y}; }

    /**
     * @brief get the centroid of triangle
     *
     * @return centroid
     */
    Pos2D getCentroid() const {
        auto [v1, v2, v3] = transformedVerticesTuple();
        return Pos2D{(v1.x + v2.x + v3.x) / 3.0f, (v1.y + v2.y + v3.y) / 3.0f};
    }

    // move old-API: move from ori_centroid to new_centroid
    /**
     * @brief move the triangle position using the new Pos2D centroid (position)
     *
     * @param ori_centroid: the original centroid, you can use the getCentroid to get it
     * @param new_centroid: the new centroid.
     */
    void move(Pos2D ori_centroid, Pos2D new_centroid) {
        auto move_x = new_centroid.x - ori_centroid.x;
        auto move_y = new_centroid.y - ori_centroid.y;  // fixed bug
        translate(move_x, move_y);
    }

    /**
     * @brief update the position of triangle using the absolute diff x, y, noted that the diff can
     * be negative
     *
     * @param x: the diff x
     * @param y: the diff y
     * @return the previous centroid before move
     */
    [[nodiscard]] Pos2D move(float x, float y) {
        auto prevC = getCentroid();
        translate(x, y);
        return prevC;
    }

    /**
     * @brief get the remain distance of triangle's top to the screen's top
     *
     * @return
     */
    float getTopPosDistRemain(windows2dSize_t /*win_size*/) {
        // assuming top edge is y = 0
        auto vs = transformedVerticesArray();
        float minY = std::min({vs[0].y, vs[1].y, vs[2].y});
        // distance to top edge: the smallest y (closest to y=0). If coords are already from
        // top-left, this equals minY.
        return std::max(0.f, minY);
    }

    float getRightPosDistRemain(windows2dSize_t win_size) {
        auto vs = transformedVerticesArray();
        float maxX = std::max({vs[0].x, vs[1].x, vs[2].x});
        return std::max(0.f, win_size.first - maxX);
    }

    float getLeftPosDistRemain(windows2dSize_t /*win_size*/) {
        auto vs = transformedVerticesArray();
        float minX = std::min({vs[0].x, vs[1].x, vs[2].x});
        return std::max(0.f, minX);
    }

    float getBottomPosDistRemain(windows2dSize_t win_size) {
        // assuming bottom edge is y = win_size.second
        auto vs = transformedVerticesArray();
        float maxY = std::max({vs[0].y, vs[1].y, vs[2].y});
        return std::max(0.f, win_size.second - maxY);
    }

    /**
     * @brief render the triangle
     */
    void render() {
        // color
        glColor4f(m_rgba.R, m_rgba.G, m_rgba.B, m_rgba.A);

        // build transformed vertices (centroid-anchored scale + translate)
        auto vs = transformedVerticesArray();

        // draw filled triangle
        glBegin(GL_TRIANGLES);
        glVertex2f(vs[0].x, vs[0].y);
        glVertex2f(vs[1].x, vs[1].y);
        glVertex2f(vs[2].x, vs[2].y);
        glEnd();
    }

   private:
    /**
     * @brief get the centroid of triangle
     *
     * @return the untransformed centroid of triangle
     */
    Pos2D baseCentroid() const {
        const auto& [v1, v2, v3] = m_vertices_base;
        return Pos2D{(v1.x + v2.x + v3.x) / 3.0f, (v1.y + v2.y + v3.y) / 3.0f};
    }

    /**
     * @brief apply the transform to the centroid
     *
     * @param v: Pos2D original centroid
     * @return the transformed centroid
     */
    Pos2D applyTransform(const Pos2D& v) const {
        Pos2D C = baseCentroid();
        float rx = (v.x - C.x) * m_scale_x;
        float ry = (v.y - C.y) * m_scale_y;
        return Pos2D{C.x + rx + m_translate_x, C.y + ry + m_translate_y};
    }

    /**
     * @brief Returns the triangle's vertices after applying scale and translation transforms.
     *
     * @return std::array<Pos2D, 3> containing the transformed vertices.
     */
    std::array<Pos2D, 3> transformedVerticesArray() const {
        const auto& [a, b, c] = m_vertices_base;
        return {applyTransform(a), applyTransform(b), applyTransform(c)};
    }

    /**
     * @brief Returns the triangle's vertices after applying scale and translation transforms.
     *
     * @return std::array<Pos2D, 3> containing the transformed vertices.
     */
    triangleVertices_t transformedVerticesTuple() const {
        auto arr = transformedVerticesArray();
        return triangleVertices_t{arr[0], arr[1], arr[2]};
    }

   private:
    // immutable geometry (we keep this as the canonical shape)
    triangleVertices_t m_vertices_base;

    // appearance
    RGBA m_rgba;

    // transform state
    float m_scale_x{1.f}, m_scale_y{1.f};  // scale about base centroid
    float m_translate_x{0.f}, m_translate_y{0.f};
};
