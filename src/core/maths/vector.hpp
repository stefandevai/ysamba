#pragma once

namespace dl
{
struct Vector2
{
  double x;
  double y;

  bool operator==(const Vector2& rhs) { return x == rhs.x && y == rhs.y; }
};

struct Vector2i
{
  int x;
  int y;

  Vector2i() {}
  Vector2i(const int x, const int y) : x(x), y(y) {}
  Vector2i(const double x, const double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}

  bool operator==(const Vector2i& rhs) { return x == rhs.x && y == rhs.y; }
};

struct Vector3
{
  double x;
  double y;
  double z;

  Vector3& operator+=(const Vector3& rhs)
  {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
  }

  bool operator==(const Vector3& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
};

struct Vector3i
{
  int x;
  int y;
  int z;

  Vector3i() : x(0), y(0), z(0) {}

  Vector3i(const int x, const int y, const int z) : x(x), y(y), z(z) {}

  Vector3i(const double x, const double y, const double z)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
  {
  }
};

struct Vector4i
{
  int x;
  int y;
  int z;
  int w;

  Vector4i() : x(0), y(0), z(0), w(0) {}

  Vector4i(const int x, const int y, const int z, const int w) : x(x), y(y), z(z), w(w) {}

  Vector4i(const double x, const double y, const double z, const int w)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)), w(static_cast<int>(w))
  {
  }
};

}  // namespace dl
