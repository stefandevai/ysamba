#pragma once

namespace dl
{
struct Vector2
{
  double x = 0.0;
  double y = 0.0;

  bool operator==(const Vector2& rhs) { return x == rhs.x && y == rhs.y; }
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

  Vector2i() : x(0), y(0) {}
  Vector2i(const int x, const int y) : x(x), y(y) {}
  Vector2i(const double x, const double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}

  bool operator==(const Vector2i& rhs) { return x == rhs.x && y == rhs.y; }
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

  Vector3i() : x(0), y(0), z(0) {}

  Vector3i(const int x, const int y, const int z) : x(x), y(y), z(z) {}

  Vector3i(const double x, const double y, const double z)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
  {
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

  bool operator==(const Vector3& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
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

  Vector4i() : x(0), y(0), z(0), w(0) {}

  Vector4i(const int x, const int y, const int z, const int w) : x(x), y(y), z(z), w(w) {}

  Vector4i(const double x, const double y, const double z, const int w)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z)), w(static_cast<int>(w))
  {
  }
};

template <typename Archive>
void serialize(Archive& archive, Vector4i& v)
{
  archive(v.x, v.y, v.z, v.w);
}

}  // namespace dl
