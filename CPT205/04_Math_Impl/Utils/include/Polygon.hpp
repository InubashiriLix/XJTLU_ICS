#pragma once

#include <GL/freeglut.h>
#include <GL/glut.h>

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

#include "basic/Color.hpp"
#include "basic/Pos.hpp"

/**
 * @brief A 2D polygon with centroid-anchored scaling and translation.
 *
 * Geometry is kept immutable in a base vertex array. Transforms are applied
 * on-the-fly for rendering and queries:
 *   v' = C + (v - C) ⊙ (sx, sy) + (tx, ty)
 * where C is the base centroid (polygon centroid via shoelace), (sx, sy) is
 * non-uniform scale, and (tx, ty) is translation.
 *
 * Coordinate convention assumed: glOrtho(0, W, H, 0, -1, 1).
 * If you use bottom-left origin, reinterpret top/bottom semantics accordingly.
 */
class Polygon2D {
   public:
    using polyVertices_t = std::vector<Pos2D>;
    using windows2dSize_t = std::pair<float, float>;

   public:
    /**
     * @brief Construct a new Polygon2D object.
     * @param vertices Base-space polygon vertices in drawing order (CW/CCW).
     * @param rgba Fill color in RGBA.
     */
    Polygon2D(polyVertices_t vertices, RGBA rgba)
        : m_vertices_base(std::move(vertices)), m_rgba(rgba) {}

    /**
     * @brief Get the base (untransformed) vertices (by value).
     * @return Copy of base vertices.
     */
    polyVertices_t getVertices() const { return m_vertices_base; }

    /**
     * @brief Overload that fills an output vector and also returns a copy.
     * @param out_vertices Output container to receive a copy of base vertices.
     * @return Copy of base vertices.
     */
    polyVertices_t getVertices(polyVertices_t& out_vertices) const {
        out_vertices = m_vertices_base;
        return m_vertices_base;
    }

    /**
     * @brief Set the base vertices (replaces geometry).
     * @param vertices New vertex array.
     */
    void setVertices(const polyVertices_t& vertices) { m_vertices_base = vertices; }

    /**
     * @brief Set the fill color.
     * @param rgba New RGBA color.
     */
    void setColor(RGBA rgba) { m_rgba = rgba; }

    /**
     * @brief Get current fill color.
     * @return RGBA color.
     */
    RGBA getColor() const { return m_rgba; }

    /**
     * @brief Set non-uniform scale around the centroid.
     * @param sx Scale along x-axis (>0).
     * @param sy Scale along y-axis (>0).
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
     * @brief Get current scale pair.
     * @return (sx, sy).
     */
    std::pair<float, float> getScale() const { return {m_scale_x, m_scale_y}; }

    /**
     * @brief Translate the polygon by (dx, dy).
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
     * @brief Get the centroid AFTER transforms.
     * @return Transformed centroid.
     */
    Pos2D getCentroid() const {
        auto tv = getTransformedVertices();
        if (tv.size() < 3) {
            // Degenerate: average of points
            Pos2D c{0.f, 0.f};
            for (auto& p : tv) {
                c.x += p.x;
                c.y += p.y;
            }
            float n = std::max<size_t>(1, tv.size());
            c.x /= n;
            c.y /= n;
            return c;
        }
        // Polygon centroid (shoelace) in transformed space
        return polygonCentroid(tv);
    }

    /**
     * @brief Move by (d_x, d_y) and return (previous, new) centroids.
     * @param d_x Delta x.
     * @param d_y Delta y.
     * @return Pair of centroids: (before, after).
     */
    std::pair<Pos2D, Pos2D> move(float d_x, float d_y) {
        auto prev = getCentroid();
        translate(d_x, d_y);
        return {prev, getCentroid()};
    }

    /**
     * @brief Distance to the top edge (y = 0), using transformed vertices.
     * @return Distance >= 0 (with above coordinate convention).
     */
    float getTopPosDistRemain() const {
        auto tv = getTransformedVertices();
        if (tv.empty()) return 0.f;
        float minY = tv[0].y;
        for (auto& p : tv) minY = std::min(minY, p.y);
        return minY;
    }

    /**
     * @brief Distance to the right edge (x = winW), using transformed vertices.
     * @param win_size (width, height).
     * @return Distance to right edge.
     */
    float getRightPosDistRemain(windows2dSize_t win_size) const {
        auto tv = getTransformedVertices();
        if (tv.empty()) return win_size.first;
        float maxX = tv[0].x;
        for (auto& p : tv) maxX = std::max(maxX, p.x);
        return win_size.first - maxX;
    }

    /**
     * @brief Distance to the left edge (x = 0), using transformed vertices.
     * @return Distance to left edge.
     */
    float getLeftPosDistRemain() const {
        auto tv = getTransformedVertices();
        if (tv.empty()) return 0.f;
        float minX = tv[0].x;
        for (auto& p : tv) minX = std::min(minX, p.x);
        return minX;
    }

    /**
     * @brief Distance to the bottom edge (y = winH), using transformed vertices.
     * @param win_size (width, height).
     * @return Distance to bottom edge.
     */
    float getBottomPosDistRemain(windows2dSize_t win_size) const {
        auto tv = getTransformedVertices();
        if (tv.empty()) return win_size.second;
        float maxY = tv[0].y;
        for (auto& p : tv) maxY = std::max(maxY, p.y);
        return win_size.second - maxY;
    }

    /**
     * @brief Render the polygon (filled) with current transforms and color.
     *
     * Assumes projection/model-view matrices are configured by the caller.
     * Uses immediate mode for simplicity.
     */
    void render() const {
        auto tv = getTransformedVertices();
        if (tv.size() < 3) return;

        glColor4f(m_rgba.R, m_rgba.G, m_rgba.B, m_rgba.A);
        glBegin(GL_POLYGON);
        for (const auto& p : tv) glVertex2f(p.x, p.y);
        glEnd();
    }

    /**
     * @brief Render a wireframe outline of the polygon.
     * @param lineWidth OpenGL line width (>=1).
     */
    void renderWireframe(float lineWidth = 1.f) const {
        auto tv = getTransformedVertices();
        if (tv.size() < 2) return;

        glLineWidth(std::max(1.f, lineWidth));
        glColor4f(m_rgba.R, m_rgba.G, m_rgba.B, m_rgba.A);

        glBegin(GL_LINE_LOOP);
        for (const auto& p : tv) glVertex2f(p.x, p.y);
        glEnd();
    }

    /**
     * @brief Get transformed vertices (same order as base vertices).
     * @return Vector of transformed vertices.
     */
    polyVertices_t getTransformedVertices() const {
        polyVertices_t out;
        out.reserve(m_vertices_base.size());
        const Pos2D C = baseCentroid();  // base centroid for transform anchor
        for (const auto& v : m_vertices_base) {
            out.push_back(applyTransform(v, C));
        }
        return out;
    }

   private:
    /**
     * @brief Base centroid (shoelace). Falls back to average if degenerate.
     * @return Centroid computed from base vertices.
     */
    Pos2D baseCentroid() const {
        if (m_vertices_base.size() < 3) {
            // Average for degenerate cases
            Pos2D c{0.f, 0.f};
            if (m_vertices_base.empty()) return c;
            for (auto& p : m_vertices_base) {
                c.x += p.x;
                c.y += p.y;
            }
            float n = static_cast<float>(m_vertices_base.size());
            c.x /= n;
            c.y /= n;
            return c;
        }
        return polygonCentroid(m_vertices_base);
    }

    /**
     * @brief Polygon centroid via shoelace formula (non self-intersecting).
     * @param poly Vertex array (closed implicitly).
     * @return Centroid in the same space as input.
     */
    static Pos2D polygonCentroid(const polyVertices_t& poly) {
        double A = 0.0;  // signed area * 2
        double Cx = 0.0;
        double Cy = 0.0;
        const size_t n = poly.size();
        for (size_t i = 0; i < n; ++i) {
            const auto& p0 = poly[i];
            const auto& p1 = poly[(i + 1) % n];
            double cross = static_cast<double>(p0.x) * p1.y - static_cast<double>(p1.x) * p0.y;
            A += cross;
            Cx += (static_cast<double>(p0.x) + p1.x) * cross;
            Cy += (static_cast<double>(p0.y) + p1.y) * cross;
        }
        if (std::abs(A) < 1e-8) {
            // Degenerate: fallback to average
            Pos2D c{0.f, 0.f};
            for (auto& p : poly) {
                c.x += p.x;
                c.y += p.y;
            }
            float inv = (poly.empty() ? 0.f : 1.f / static_cast<float>(poly.size()));
            c.x *= inv;
            c.y *= inv;
            return c;
        }
        double invA = 1.0 / (3.0 * A);
        return Pos2D{static_cast<float>(Cx * invA), static_cast<float>(Cy * invA)};
    }

    /**
     * @brief Apply centroid-anchored scaling and translation to a point.
     * @param v Base-space vertex.
     * @param C Base centroid used as the anchor.
     * @return Transformed point.
     *
     * v' = C + (v - C) ⊙ (sx, sy) + (tx, ty)
     */
    Pos2D applyTransform(const Pos2D& v, const Pos2D& C) const {
        float rx = (v.x - C.x) * m_scale_x;
        float ry = (v.y - C.y) * m_scale_y;
        return {C.x + rx + m_translate_x, C.y + ry + m_translate_y};
    }

   private:
    polyVertices_t m_vertices_base;  ///< Base geometry (immutable w.r.t. transforms).
    RGBA m_rgba;                     ///< Fill color.

    float m_scale_x{1.f};      ///< Scale on x-axis (centroid-anchored).
    float m_scale_y{1.f};      ///< Scale on y-axis (centroid-anchored).
    float m_translate_x{0.f};  ///< Translation on x-axis.
    float m_translate_y{0.f};  ///< Translation on y-axis.
};
