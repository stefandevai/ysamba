#include "./vector.hpp"

#include <tuple>

namespace dl
{
Vector2::Vector2(const double x, const double y) : x(x), y(y) {}

Vector2::Vector2(const int x, const int y) : x(static_cast<double>(x)), y(static_cast<double>(y)) {}

Vector2::Vector2(const Vector2& other)
{
  x = other.x;
  y = other.y;
}

Vector2::Vector2(const Vector2i& other)
{
  x = other.x;
  y = other.y;
}

bool Vector2::operator==(const Vector2& rhs) const { return (x == rhs.x) && (y == rhs.y); }

bool Vector2::operator<(const Vector2& rhs) const { return std::tie(x, y) < std::tie(rhs.x, rhs.y); }

bool Vector2::operator<=(const Vector2& rhs) const { return !(rhs < *this); }

bool Vector2::operator>(const Vector2& rhs) const { return rhs < *this; }

bool Vector2::operator>=(const Vector2& rhs) const { return !(*this < rhs); }

Vector2& Vector2::operator=(const Vector2& rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2& Vector2::operator=(const Vector2i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2 Vector2::operator+(const Vector2& rhs) const { return Vector2{x + rhs.x, y + rhs.y}; }

Vector2& Vector2::operator+=(const Vector2& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Vector2 Vector2::operator-(const Vector2& rhs) const { return Vector2{x - rhs.x, y - rhs.y}; }

Vector2& Vector2::operator-=(const Vector2& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vector2 Vector2::operator*(const double rhs) const { return Vector2{x * rhs, y * rhs}; }

Vector2& Vector2::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector2i::Vector2i(const int x, const int y) : x(x), y(y) {}

Vector2i::Vector2i(const double x, const double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}

Vector2i::Vector2i(const Vector2i& other)
{
  x = other.x;
  y = other.y;
}

Vector2i::Vector2i(const Vector2& other)
{
  x = other.x;
  y = other.y;
}

bool Vector2i::operator==(const Vector2i& rhs) const { return (y == rhs.y) && (x == rhs.x); }

bool Vector2i::operator!=(const Vector2i& rhs) const { return !(*this == rhs); }

bool Vector2i::operator<(const Vector2i& rhs) const { return std::tie(x, y) < std::tie(rhs.x, rhs.y); }

bool Vector2i::operator<=(const Vector2i& rhs) const { return !(rhs < *this); }

bool Vector2i::operator>(const Vector2i& rhs) const { return rhs < *this; }

bool Vector2i::operator>=(const Vector2i& rhs) const { return !(*this < rhs); }

Vector2i& Vector2i::operator=(const Vector2i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2i& Vector2i::operator=(const Vector2& rhs)
{
  x = rhs.x;
  y = rhs.y;
  return *this;
}

Vector2i Vector2i::operator+(const Vector2i& rhs) const { return Vector2i{x + rhs.x, y + rhs.y}; }

Vector2i& Vector2i::operator+=(const Vector2i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  return *this;
}

Vector2i Vector2i::operator-(const Vector2i& rhs) const { return Vector2i{x - rhs.x, y - rhs.y}; }

Vector2i& Vector2i::operator-=(const Vector2i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  return *this;
}

Vector2i Vector2i::operator*(const int rhs) const { return Vector2i{x * rhs, y * rhs}; }

Vector2i& Vector2i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  return *this;
}

Vector3i::Vector3i(const int x, const int y, const int z) : x(x), y(y), z(z) {}

Vector3i::Vector3i(const double x, const double y, const double z)
    : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
{
}

Vector3i::Vector3i(const Vector3i& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
}

Vector3i::Vector3i(const Vector3& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
}

bool Vector3i::operator==(const Vector3i& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

bool Vector3i::operator<(const Vector3i& rhs) const { return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z); }

bool Vector3i::operator<=(const Vector3i& rhs) const { return !(rhs < *this); }

bool Vector3i::operator>(const Vector3i& rhs) const { return rhs < *this; }

bool Vector3i::operator>=(const Vector3i& rhs) const { return !(*this < rhs); }

Vector3i& Vector3i::operator=(const Vector3i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3i& Vector3i::operator=(const Vector3& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3i Vector3i::operator+(const Vector3i& rhs) const { return Vector3i{x + rhs.x, y + rhs.y, z + rhs.z}; }

Vector3i& Vector3i::operator+=(const Vector3i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3i Vector3i::operator-(const Vector3i& rhs) const { return Vector3i{x - rhs.x, y - rhs.y, z - rhs.z}; }

Vector3i& Vector3i::operator-=(const Vector3i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3i Vector3i::operator*(const int rhs) const { return Vector3i{x * rhs, y * rhs, z * rhs}; }

Vector3i& Vector3i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector3::Vector3(const double x, const double y, const double z) : x(x), y(y), z(z) {}

Vector3::Vector3(const int x, const int y, const int z)
    : x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z))
{
}

Vector3::Vector3(const Vector3& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
}

Vector3::Vector3(const Vector3i& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
}

bool Vector3::operator==(const Vector3& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

bool Vector3::operator<(const Vector3& rhs) const { return std::tie(x, y, z) < std::tie(rhs.x, rhs.y, rhs.z); }

bool Vector3::operator<=(const Vector3& rhs) const { return !(rhs < *this); }

bool Vector3::operator>(const Vector3& rhs) const { return rhs < *this; }

bool Vector3::operator>=(const Vector3& rhs) const { return !(*this < rhs); }

Vector3& Vector3::operator=(const Vector3& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3& Vector3::operator=(const Vector3i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  return *this;
}

Vector3 Vector3::operator+(const Vector3& rhs) const { return Vector3{x + rhs.x, y + rhs.y, z + rhs.z}; }

Vector3& Vector3::operator+=(const Vector3& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  return *this;
}

Vector3 Vector3::operator-(const Vector3& rhs) const { return Vector3{x - rhs.x, y - rhs.y, z - rhs.z}; }

Vector3& Vector3::operator-=(const Vector3& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  return *this;
}

Vector3 Vector3::operator*(const double rhs) const { return Vector3{x * rhs, y * rhs, z * rhs}; }

Vector3& Vector3::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  return *this;
}

Vector4d::Vector4d(const double x, const double y, const double z, const double w) : x(x), y(y), z(z), w(w) {}

Vector4d::Vector4d(const int x, const int y, const int z, const int w)
    : x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z)), w(static_cast<double>(w))
{
}

Vector4d::Vector4d(const Vector4d& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

Vector4d::Vector4d(const Vector4i& other)
{
  x = static_cast<double>(other.x);
  y = static_cast<double>(other.y);
  z = static_cast<double>(other.z);
  w = static_cast<double>(other.w);
}

bool Vector4d::operator==(const Vector4d& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

bool Vector4d::operator<(const Vector4d& rhs) const
{
  return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}

bool Vector4d::operator<=(const Vector4d& rhs) const { return !(rhs < *this); }

bool Vector4d::operator>(const Vector4d& rhs) const { return rhs < *this; }

bool Vector4d::operator>=(const Vector4d& rhs) const { return !(*this < rhs); }

Vector4d& Vector4d::operator=(const Vector4d& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

Vector4d Vector4d::operator+(const Vector4d& rhs) const { return Vector4d{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }

Vector4d& Vector4d::operator+=(const Vector4d& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vector4d Vector4d::operator-(const Vector4d& rhs) const { return Vector4d{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }

Vector4d& Vector4d::operator-=(const Vector4d& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

Vector4d Vector4d::operator*(const double rhs) const { return Vector4d{x * rhs, y * rhs, z * rhs, w * rhs}; }

Vector4d& Vector4d::operator*=(const double rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

Vector4i::Vector4i(const int x, const int y, const int z, const int w) : x(x), y(y), z(z), w(w) {}

Vector4i::Vector4i(const double x, const double y, const double z, const double w)
    : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)), w(static_cast<int>(w))
{
}

Vector4i::Vector4i(const Vector4i& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

Vector4i::Vector4i(const Vector4d& other)
{
  x = static_cast<int>(other.x);
  y = static_cast<int>(other.y);
  z = static_cast<int>(other.z);
  w = static_cast<int>(other.w);
}

bool Vector4i::operator==(const Vector4i& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

bool Vector4i::operator<(const Vector4i& rhs) const
{
  return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w);
}

bool Vector4i::operator<=(const Vector4i& rhs) const { return !(rhs < *this); }

bool Vector4i::operator>(const Vector4i& rhs) const { return rhs < *this; }

bool Vector4i::operator>=(const Vector4i& rhs) const { return !(*this < rhs); }

Vector4i& Vector4i::operator=(const Vector4i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

Vector4i Vector4i::operator+(const Vector4i& rhs) const { return Vector4i{x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w}; }

Vector4i& Vector4i::operator+=(const Vector4i& rhs)
{
  x += rhs.x;
  y += rhs.y;
  z += rhs.z;
  w += rhs.w;
  return *this;
}

Vector4i Vector4i::operator-(const Vector4i& rhs) const { return Vector4i{x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w}; }

Vector4i& Vector4i::operator-=(const Vector4i& rhs)
{
  x -= rhs.x;
  y -= rhs.y;
  z -= rhs.z;
  w -= rhs.w;
  return *this;
}

Vector4i Vector4i::operator*(const int rhs) const { return Vector4i{x * rhs, y * rhs, z * rhs, w * rhs}; }

Vector4i& Vector4i::operator*=(const int rhs)
{
  x *= rhs;
  y *= rhs;
  z *= rhs;
  w *= rhs;
  return *this;
}

}  // namespace dl
