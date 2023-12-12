#pragma once

namespace dl
{
struct Vector2
{
  double x = 0.0;
  double y = 0.0;

  Vector2() = default;
  Vector2(const double x, const double y) : x(x), y(y) {}
  Vector2(const int x, const int y) : x(static_cast<double>(x)), y(static_cast<double>(y)) {}
  Vector2(const Vector2& other)
  {
    x = other.x;
    y = other.y;
  }
  bool operator==(const Vector2& rhs) { return x == rhs.x && y == rhs.y; }
  Vector2& operator=(const Vector2& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector2& v)
{
  archive(v.x, v.y);
}

struct Vector2i
{
  int x = 0;
  int y = 0;

  Vector2i() = default;
  Vector2i(const int x, const int y) : x(x), y(y) {}
  Vector2i(const double x, const double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}
  Vector2i(const Vector2i& other)
  {
    x = other.x;
    y = other.y;
  }

  bool operator==(const Vector2i& rhs) { return x == rhs.x && y == rhs.y; }
  Vector2i& operator=(const Vector2i& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    return *this;
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector2i& v)
{
  archive(v.x, v.y);
}

struct Vector3i
{
  int x = 0;
  int y = 0;
  int z = 0;

  Vector3i() = default;
  Vector3i(const int x, const int y, const int z) : x(x), y(y), z(z) {}
  Vector3i(const double x, const double y, const double z)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
  {
  }
  Vector3i(const Vector3i& other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
  }
  bool operator==(const Vector3i& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
  Vector3i& operator=(const Vector3i& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector3i& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector3
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  Vector3& operator+=(const Vector3& rhs)
  {
    this->x += rhs.x;
    this->y += rhs.y;
    this->z += rhs.z;
    return *this;
  }

  Vector3() = default;
  Vector3(const double x, const double y, const double z) : x(x), y(y), z(z) {}
  Vector3(const int x, const int y, const int z)
      : x(static_cast<double>(x)), y(static_cast<double>(y)), z(static_cast<double>(z))
  {
  }
  Vector3(const Vector3& other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
  }
  bool operator==(const Vector3& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
  Vector3& operator=(const Vector3& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector3& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector4i
{
  int x = 0;
  int y = 0;
  int z = 0;
  int w = 0;

  Vector4i() = default;

  Vector4i(const int x, const int y, const int z, const int w) : x(x), y(y), z(z), w(w) {}

  Vector4i(const double x, const double y, const double z, const int w)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)), w(static_cast<int>(w))
  {
  }

  Vector4i(const Vector4i& other)
  {
    x = other.x;
    y = other.y;
    z = other.z;
    w = other.w;
  }
  bool operator==(const Vector4i& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
  Vector4i& operator=(const Vector4i& rhs)
  {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    w = rhs.w;
    return *this;
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector4i& v)
{
  archive(v.x, v.y, v.z, v.w);
}

}  // namespace dl
