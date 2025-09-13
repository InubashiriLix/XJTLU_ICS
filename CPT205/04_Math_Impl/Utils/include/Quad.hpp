#pragma once

#include <GL/freeglut.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <tuple>
#include <utility>

#include "basic/Color.hpp"
#include "basic/Pos.hpp"

/**
 * @brief A 2D quadrilateral with centroid-anchored scaling and translation.
 *
 * Geometry is stored in a base tuple and never overwritten by transforms.
 * Rendering and queries apply transforms on-the-fly:
 *   v' = C + (v - C) ⊙ S + T
 * where C is the base centroid, S = (sx, sy), and T = (tx, ty).
 *
 * Coordinate convention is assumed to be glOrtho(0, W, H, 0, -1, 1):
 *   top-left is (0,0), x rightwards, y downwards.
 */
class Quad2D {
   public:
    using rectVertices_t = std::tuple<Pos2D, Pos2D, Pos2D, Pos2D>;
    using windows2dSize_t = std::pair<float, float>;

   public:
    /**
     * @brief Construct a new Quad2D object.
     * @param vertices Four vertices of the quad (tuple order is preserved as given).
     * @param rgba Fill color in RGBA.
     */
    Quad2D(rectVertices_t vertices, RGBA rgba) : m_vertices_base(vertices), m_rgba(rgba) {}

    /**
     * @brief Get the base (untransformed) vertices.
     * @return Tuple of four base vertices.
     */
    rectVertices_t getVertices() const { return m_vertices_base; }

    /**
     * @brief Set the base vertices (replaces geometry).
     * @param vertices Tuple of four vertices.
     */
    void setVertices(rectVertices_t vertices) { m_vertices_base = vertices; }

    /**
     * @brief Set the fill color.
     * @param rgba New RGBA color.
     */
    void setColor(RGBA rgba) { m_rgba = rgba; }

    /**
     * @brief Get the current fill color.
     * @return RGBA color.
     */
    RGBA getColor() const { return m_rgba; }

    /**
     * @brief Set non-uniform scale around the centroid.
     * @param sx Scale on x-axis (>0).
     * @param sy Scale on y-axis (>0).
     *
     * Values <= 0 are clamped to 1 to avoid invalid scales.
     */
    void setScale(float sx, float sy) {
        m_scale_x = (sx <= 0.f ? 1.f : sx);
        m_scale_y = (sy <= 0.f ? 1.f : sy);
    }

    /**
     * @brief Set uniform scale around the centroid.
     * @param s Uniform scale (>0).
     */
    void setUniformScale(float s) { setScale(s, s); }

    /**
     * @brief Get the current scale pair.
     * @return (sx, sy).
     */
    std::pair<float, float> getScale() const { return {m_scale_x, m_scale_y}; }

    /**
     * @brief Translate the quad by (dx, dy).
     * @param dx Delta on x-axis.
     * @param dy Delta on y-axis.
     */
    void translate(float dx, float dy) {
        m_translate_x += dx;
        m_translate_y += dy;
    }

    /**
     * @brief Set the absolute translation.
     * @param tx Translation on x-axis.
     * @param ty Translation on y-axis.
     */
    void setTranslate(float tx, float ty) {
        m_translate_x = tx;
        m_translate_y = ty;
    }

    /**
     * @brief Get the current translation.
     * @return (tx, ty).
     */
    std::pair<float, float> getTranslate() const { return {m_translate_x, m_translate_y}; }

    /**
     * @brief Get the centroid after transforms.
     * @return Transformed centroid position.
     */
    Pos2D getCentroid() const {
        auto [a, b, c, d] = transformedVerticesTuple();
        float cx = (a.x + b.x + c.x + d.x) / 4.0f;
        float cy = (a.y + b.y + c.y + d.y) / 4.0f;
        return {cx, cy};
    }

    /**
     * @brief Translate the quad by (dx, dy) and return (previous, new) centroids.
     * @param d_x Delta on x-axis.
     * @param d_y Delta on y-axis.
     * @return Pair of (previous centroid, new centroid).
     */
    std::pair<Pos2D, Pos2D> move(float d_x, float d_y) {
        auto prev = getCentroid();
        translate(d_x, d_y);
        return {prev, getCentroid()};
    }

    /**
     * @brief Distance to the top edge (y = 0), using transformed vertices.
     * @return Non-negative distance (clamped by projection usage).
     */
    float getTopPosDistRemain() const {
        auto [a, b, c, d] = transformedVerticesTuple();
        float minY = std::min(std::min(a.y, b.y), std::min(c.y, d.y));
        return minY;
    }

    /**
     * @brief Distance to the right edge (x = winW), using transformed vertices.
     * @param win_size Window size (width, height).
     * @return Distance to the right edge.
     */
    float getRightPosDistRemain(windows2dSize_t win_size) const {
        auto [a, b, c, d] = transformedVerticesTuple();
        float maxX = std::max(std::max(a.x, b.x), std::max(c.x, d.x));
        return win_size.first - maxX;
    }

    /**
     * @brief Distance to the left edge (x = 0), using transformed vertices.
     * @return Distance to the left edge.
     */
    float getLeftPosDistRemain() const {
        auto [a, b, c, d] = transformedVerticesTuple();
        float minX = std::min(std::min(a.x, b.x), std::min(c.x, d.x));
        return minX;
    }

    /**
     * @brief Distance to the bottom edge (y = winH), using transformed vertices.
     * @param win_size Window size (width, height).
     * @return Distance to the bottom edge.
     */
    float getBottomPosDistRemain(windows2dSize_t win_size) const {
        auto [a, b, c, d] = transformedVerticesTuple();
        float maxY = std::max(std::max(a.y, b.y), std::max(c.y, d.y));
        return win_size.second - maxY;
    }

    /**
     * @brief Render the quad (filled) with current transforms and color.
     *
     * Assumes projection/model-view matrices are already configured by caller.
     * Uses immediate mode for simplicity.
     */
    void render() const {
        glColor4f(m_rgba.R, m_rgba.G, m_rgba.B, m_rgba.A);

        auto [v0, v1, v2, v3] = transformedVerticesTuple();

        glBegin(GL_QUADS);
        glVertex2f(v0.x, v0.y);
        glVertex2f(v1.x, v1.y);
        glVertex2f(v2.x, v2.y);
        glVertex2f(v3.x, v3.y);
        glEnd();
    }

   private:
    /**
     * @brief Base (untransformed) centroid.
     * @return Centroid computed from base vertices.
     */
    Pos2D baseCentroid() const {
        const auto& [v0, v1, v2, v3] = m_vertices_base;
        float cx = (v0.x + v1.x + v2.x + v3.x) / 4.0f;
        float cy = (v0.y + v1.y + v2.y + v3.y) / 4.0f;
        return {cx, cy};
    }

    /**
     * @brief Apply centroid-anchored scaling and translation to a point.
     * @param v Base-space vertex.
     * @return Transformed vertex.
     *
     * v' = C + (v - C) ⊙ (sx, sy) + (tx, ty)
     */
    Pos2D applyTransform(const Pos2D& v) const {
        Pos2D C = baseCentroid();
        float rx = (v.x - C.x) * m_scale_x;
        float ry = (v.y - C.y) * m_scale_y;
        return {C.x + rx + m_translate_x, C.y + ry + m_translate_y};
    }

    /**
     * @brief Get transformed vertices as a tuple (same order as base tuple).
     * @return Tuple of four transformed vertices.
     */
    rectVertices_t transformedVerticesTuple() const {
        const auto& [a, b, c, d] = m_vertices_base;
        Pos2D ta = applyTransform(a);
        Pos2D tb = applyTransform(b);
        Pos2D tc = applyTransform(c);
        Pos2D td = applyTransform(d);
        return rectVertices_t{ta, tb, tc, td};
    }

   private:
    rectVertices_t m_vertices_base;  ///< Base geometry (not overwritten by transforms).
    RGBA m_rgba;                     ///< Fill color.

    float m_scale_x{1.f};      ///< Scale on x-axis (centroid-anchored).
    float m_scale_y{1.f};      ///< Scale on y-axis (centroid-anchored).
    float m_translate_x{0.f};  ///< Translation on x-axis.
    float m_translate_y{0.f};  ///< Translation on y-axis.
};
