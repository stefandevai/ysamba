#pragma once

namespace dl
{
struct Vector2i;
struct Vector3i;
struct Vector4i;

struct Vector2
{
  double x = 0.0;
  double y = 0.0;

  Vector2() = default;
  Vector2(const double x, const double y);
  Vector2(const int x, const int y);
  Vector2(const Vector2& other);
  Vector2(const Vector2i& other);

  bool operator==(const Vector2& rhs) const;
  Vector2& operator=(const Vector2& rhs);
  Vector2& operator=(const Vector2i& rhs);
  Vector2 operator+(const Vector2& rhs);
  Vector2& operator+=(const Vector2& rhs);
  Vector2 operator-(const Vector2& rhs);
  Vector2& operator-=(const Vector2& rhs);
  Vector2 operator*(const double rhs);
  Vector2& operator*=(const double rhs);
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
  Vector2i(const int x, const int y);
  Vector2i(const double x, const double y);
  Vector2i(const Vector2i& other);
  Vector2i(const Vector2& other);

  bool operator==(const Vector2i& rhs) const;
  Vector2i& operator=(const Vector2i& rhs);
  Vector2i& operator=(const Vector2& rhs);
  Vector2i operator+(const Vector2i& rhs);
  Vector2i& operator+=(const Vector2i& rhs);
  Vector2i operator-(const Vector2i& rhs);
  Vector2i& operator-=(const Vector2i& rhs);
  Vector2i operator*(const int rhs);
  Vector2i& operator*=(const int rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector2i& v)
{
  archive(v.x, v.y);
}

struct Vector3
{
  double x = 0.0;
  double y = 0.0;
  double z = 0.0;

  Vector3() = default;
  Vector3(const double x, const double y, const double z);
  Vector3(const int x, const int y, const int z);
  Vector3(const Vector3& other);
  Vector3(const Vector3i& other);

  bool operator==(const Vector3& rhs) const;
  Vector3& operator=(const Vector3& rhs);
  Vector3& operator=(const Vector3i& rhs);
  Vector3 operator+(const Vector3& rhs);
  Vector3& operator+=(const Vector3& rhs);
  Vector3 operator-(const Vector3& rhs);
  Vector3& operator-=(const Vector3& rhs);
  Vector3 operator*(const double rhs);
  Vector3& operator*=(const double rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector3& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector3i
{
  int x = 0;
  int y = 0;
  int z = 0;

  Vector3i() = default;
  Vector3i(const int x, const int y, const int z);
  Vector3i(const double x, const double y, const double z);
  Vector3i(const Vector3i& other);
  Vector3i(const Vector3& other);

  bool operator==(const Vector3i& rhs) const;
  Vector3i& operator=(const Vector3i& rhs);
  Vector3i& operator=(const Vector3& rhs);
  Vector3i operator+(const Vector3i& rhs);
  Vector3i& operator+=(const Vector3i& rhs);
  Vector3i operator-(const Vector3i& rhs);
  Vector3i& operator-=(const Vector3i& rhs);
  Vector3i operator*(const int rhs);
  Vector3i& operator*=(const int rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector3i& v)
{
  archive(v.x, v.y, v.z);
}

struct Vector4d
{
  double x = 0;
  double y = 0;
  double z = 0;
  double w = 0;

  Vector4d() = default;
  Vector4d(const double x, const double y, const double z, const double w);
  Vector4d(const int x, const int y, const int z, const int w);
  Vector4d(const Vector4d& other);
  Vector4d(const Vector4i& other);

  bool operator==(const Vector4d& rhs) const;
  Vector4d& operator=(const Vector4d& rhs);
  Vector4d operator+(const Vector4d& rhs);
  Vector4d& operator+=(const Vector4d& rhs);
  Vector4d operator-(const Vector4d& rhs);
  Vector4d& operator-=(const Vector4d& rhs);
  Vector4d operator*(const double rhs);
  Vector4d& operator*=(const double rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector4d& v)
{
  archive(v.x, v.y, v.z, v.w);
}

struct Vector4i
{
  int x = 0;
  int y = 0;
  int z = 0;
  int w = 0;

  Vector4i() = default;
  Vector4i(const int x, const int y, const int z, const int w);
  Vector4i(const double x, const double y, const double z, const double w);
  Vector4i(const Vector4i& other);
  Vector4i(const Vector4d& other);

  bool operator==(const Vector4i& rhs) const;
  Vector4i& operator=(const Vector4i& rhs);
  Vector4i operator+(const Vector4i& rhs);
  Vector4i& operator+=(const Vector4i& rhs);
  Vector4i operator-(const Vector4i& rhs);
  Vector4i& operator-=(const Vector4i& rhs);
  Vector4i operator*(const int rhs);
  Vector4i& operator*=(const int rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector4i& v)
{
  archive(v.x, v.y, v.z, v.w);
}

}  // namespace dl
