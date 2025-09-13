#pragma once
#include <GL/freeglut.h>
#include <GL/glut.h>

#include <cassert>

#include "utils/clamp.hpp"

struct RGB {
    float R;  // the red channel
    float G;  // the green channel
    float B;  // the blue channel

    RGB(float r, float g, float b) : R(r), G(g), B(b) {
        auto ok = [](float x) { return 0.0f <= x && x <= 1.0f; };
        assert(ok(r) && "r in [0,1]");
        assert(ok(g) && "g in [0,1]");
        assert(ok(b) && "b in [0,1]");
    }

    RGB operator+(const RGB& other) {
        RGB newRGB(clamp(this->R + other.R, 0.f, 1.f), clamp(this->G + other.G, 0.f, 1.f),
                   clamp(this->B + other.B, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGB operator-(const RGB& other) {
        RGB newRGB(clamp(this->R - other.R, 0.f, 1.f), clamp(this->G - other.G, 0.f, 1.f),
                   clamp(this->B - other.B, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGB operator*(const float scaler) {
        RGB newRGB(clamp(this->R * scaler, 0.f, 1.f), clamp(this->G * scaler, 0.f, 1.f),
                   clamp(this->B * scaler, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGB operator/(const float denominator) {
        assert(denominator == 0 && "the denominator cannnot be 0 !!!");
        RGB newRGB(clamp(this->R / denominator, 0.f, 1.f), clamp(this->G / denominator, 0.f, 1.f),
                   clamp(this->B / denominator, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGB& operator+=(const RGB& other) {
        this->R = clamp(this->R + other.R, 0.f, 1.f);
        this->G = clamp(this->G + other.G, 0.f, 1.f);
        this->B = clamp(this->B + other.B, 0.f, 1.f);
        return *this;
    }

    void render() { glColor3f(R, G, B); }
};

struct RGBA {
    float R;  // the red channel
    float G;  // the green channel
    float B;  // the blue channel
    float A;  // the transparent channel

    RGBA(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {
        auto ok = [](float x) { return 0.0f <= x && x <= 1.0f; };
        assert(ok(r) && "r in [0,1]");
        assert(ok(g) && "g in [0,1]");
        assert(ok(b) && "b in [0,1]");
        assert(ok(a) && "a in [0,1]");
    }

    RGBA operator+(const RGBA& other) {
        RGBA newRGB(clamp(this->R + other.R, 0.f, 1.f), clamp(this->G + other.G, 0.f, 1.f),
                    clamp(this->B + other.B, 0.f, 1.f), clamp(this->A + other.A, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGBA operator-(const RGBA& other) {
        RGBA newRGB(clamp(this->R - other.R, 0.f, 1.f), clamp(this->G - other.G, 0.f, 1.f),
                    clamp(this->B - other.B, 0.f, 1.f), clamp(this->A - other.A, 0.f, 1.f));
        return std::move(newRGB);
    }

    RGBA operator*(const float scaler) {
        RGBA newRGBA(clamp(this->R * scaler, 0.f, 1.f), clamp(this->G * scaler, 0.f, 1.f),
                     clamp(this->B * scaler, 0.f, 1.f), clamp(this->A * scaler, 0.f, 1.f));
        return std::move(newRGBA);
    }

    RGBA operator/(const float denominator) {
        assert(denominator == 0 && "the denominator cannnot be 0 !!!");
        RGBA newRGBA(clamp(this->R / denominator, 0.f, 1.f), clamp(this->G / denominator, 0.f, 1.f),
                     clamp(this->B / denominator, 0.f, 1.f),
                     clamp(this->A / denominator, 0.f, 1.f));
        return std::move(newRGBA);
    }

    RGBA& operator+=(const RGBA& other) {
        this->R = clamp(this->R + other.R, 0.f, 1.f);
        this->G = clamp(this->G + other.G, 0.f, 1.f);
        this->B = clamp(this->B + other.B, 0.f, 1.f);
        this->A = clamp(this->A + other.A, 0.f, 1.f);
        return *this;
    }

    void render() { glColor4f(R, G, B, A); }
};
