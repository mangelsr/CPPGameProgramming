#include <math.h>

#include "Vec2.h"

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{
}

bool Vec2::operator==(const Vec2 &rhs) const
{
    return x == rhs.x && y == rhs.y;
}

bool Vec2::operator!=(const Vec2 &rhs) const
{
    return x != rhs.x || y != rhs.y;
}

Vec2 Vec2::operator+(const Vec2 &rhs) const
{
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2 &rhs) const
{
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
    return Vec2(x / val, y / val);
}

Vec2 Vec2::operator*(const float val) const
{
    return Vec2(x * val, y * val);
}

void Vec2::operator+=(const Vec2 &rhs)
{
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator-=(const Vec2 &rhs)
{
    x -= rhs.x;
    y -= rhs.y;
}

void Vec2::operator*=(const float val)
{
    x *= val;
    y *= val;
}

void Vec2::operator/=(const float val)
{
    x /= val;
    y /= val;
}

float Vec2::dist(const Vec2 &rhs) const
{
    Vec2 D = Vec2(rhs.x - x, rhs.y - y);
    return sqrtf(D.x * D.x + D.y * D.y);
}

float Vec2::module() const
{
    return sqrtf(x * x + y * y);
}

Vec2 Vec2::normalize() const
{
    float m = module();
    return Vec2(x / m, y / m);
}

float Vec2::cross(Vec2 &rhs) const
{
    return x * rhs.y - y * rhs.x;
}

std::ostream &operator<<(std::ostream &os, const Vec2 &vec)
{
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}