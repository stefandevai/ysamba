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
  ~Vector2() = default;
  Vector2(double x, double y) noexcept;
  Vector2(int x, int y) noexcept;
  Vector2(const Vector2& other) noexcept;
  Vector2(const Vector2i& other) noexcept;
  Vector2(Vector2&& other) noexcept;

  Vector2& operator=(const Vector2& rhs) noexcept;
  Vector2& operator=(const Vector2i& rhs) noexcept;
  Vector2& operator=(Vector2&& rhs) noexcept;
  bool operator==(const Vector2& rhs) const;
  bool operator<(const Vector2& rhs) const;
  bool operator<=(const Vector2& rhs) const;
  bool operator>(const Vector2& rhs) const;
  bool operator>=(const Vector2& rhs) const;
  Vector2 operator+(const Vector2& rhs) const;
  Vector2& operator+=(const Vector2& rhs);
  Vector2 operator-(const Vector2& rhs) const;
  Vector2& operator-=(const Vector2& rhs);
  Vector2 operator*(double rhs) const;
  Vector2& operator*=(double rhs);
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
  ~Vector2i() = default;
  Vector2i(int x, int y) noexcept;
  Vector2i(double x, double y) noexcept;
  Vector2i(const Vector2i& other) noexcept;
  Vector2i(const Vector2& other) noexcept;
  Vector2i(Vector2i&& other) noexcept;

  Vector2i& operator=(const Vector2i& rhs) noexcept;
  Vector2i& operator=(const Vector2& rhs) noexcept;
  Vector2i& operator=(Vector2i&& rhs) noexcept;
  bool operator==(const Vector2i& rhs) const;
  bool operator!=(const Vector2i& rhs) const;
  bool operator<(const Vector2i& rhs) const;
  bool operator<=(const Vector2i& rhs) const;
  bool operator>(const Vector2i& rhs) const;
  bool operator>=(const Vector2i& rhs) const;
  Vector2i operator+(const Vector2i& rhs) const;
  Vector2i& operator+=(const Vector2i& rhs);
  Vector2i operator-(const Vector2i& rhs) const;
  Vector2i& operator-=(const Vector2i& rhs);
  Vector2i operator*(int rhs) const;
  Vector2i& operator*=(int rhs);
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
  ~Vector3() = default;
  Vector3(double x, double y, double z) noexcept;
  Vector3(int x, int y, int z) noexcept;
  Vector3(const Vector3& other) noexcept;
  Vector3(const Vector3i& other) noexcept;
  Vector3(Vector3&& other) noexcept;

  Vector3& operator=(const Vector3& rhs) noexcept;
  Vector3& operator=(const Vector3i& rhs) noexcept;
  Vector3& operator=(Vector3&& rhs) noexcept;
  bool operator==(const Vector3& rhs) const;
  bool operator<(const Vector3& rhs) const;
  bool operator<=(const Vector3& rhs) const;
  bool operator>(const Vector3& rhs) const;
  bool operator>=(const Vector3& rhs) const;
  Vector3 operator+(const Vector3& rhs) const;
  Vector3& operator+=(const Vector3& rhs);
  Vector3 operator-(const Vector3& rhs) const;
  Vector3& operator-=(const Vector3& rhs);
  Vector3 operator*(double rhs) const;
  Vector3& operator*=(double rhs);
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
  ~Vector3i() = default;
  Vector3i(int x, int y, int z) noexcept;
  Vector3i(double x, double y, double z) noexcept;
  Vector3i(const Vector3i& other) noexcept;
  Vector3i(const Vector3& other) noexcept;
  Vector3i(Vector3i&& other) noexcept;

  Vector3i& operator=(const Vector3i& rhs) noexcept;
  Vector3i& operator=(const Vector3& rhs) noexcept;
  Vector3i& operator=(Vector3i&& rhs) noexcept;
  bool operator==(const Vector3i& rhs) const;
  bool operator<(const Vector3i& rhs) const;
  bool operator<=(const Vector3i& rhs) const;
  bool operator>(const Vector3i& rhs) const;
  bool operator>=(const Vector3i& rhs) const;
  Vector3i operator+(const Vector3i& rhs) const;
  Vector3i& operator+=(const Vector3i& rhs);
  Vector3i operator-(const Vector3i& rhs) const;
  Vector3i& operator-=(const Vector3i& rhs);
  Vector3i operator*(int rhs) const;
  Vector3i& operator*=(int rhs);
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
  ~Vector4d() = default;
  Vector4d(double x, double y, double z, double w) noexcept;
  Vector4d(int x, int y, int z, int w) noexcept;
  Vector4d(const Vector4d& other) noexcept;
  Vector4d(const Vector4i& other) noexcept;
  Vector4d(Vector4d&& other) noexcept;

  Vector4d& operator=(const Vector4d& rhs) noexcept;
  Vector4d& operator=(Vector4d&& rhs) noexcept;
  bool operator==(const Vector4d& rhs) const;
  bool operator<(const Vector4d& rhs) const;
  bool operator<=(const Vector4d& rhs) const;
  bool operator>(const Vector4d& rhs) const;
  bool operator>=(const Vector4d& rhs) const;
  Vector4d operator+(const Vector4d& rhs) const;
  Vector4d& operator+=(const Vector4d& rhs);
  Vector4d operator-(const Vector4d& rhs) const;
  Vector4d& operator-=(const Vector4d& rhs);
  Vector4d operator*(double rhs) const;
  Vector4d& operator*=(double rhs);
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
  ~Vector4i() = default;
  Vector4i(int x, int y, int z, int w) noexcept;
  Vector4i(double x, double y, double z, double w) noexcept;
  Vector4i(const Vector4i& other) noexcept;
  Vector4i(const Vector4d& other) noexcept;
  Vector4i(Vector4i&& other) noexcept;

  Vector4i& operator=(const Vector4i& rhs) noexcept;
  Vector4i& operator=(Vector4i&& rhs) noexcept;
  bool operator==(const Vector4i& rhs) const;
  bool operator<(const Vector4i& rhs) const;
  bool operator<=(const Vector4i& rhs) const;
  bool operator>(const Vector4i& rhs) const;
  bool operator>=(const Vector4i& rhs) const;
  Vector4i operator+(const Vector4i& rhs) const;
  Vector4i& operator+=(const Vector4i& rhs);
  Vector4i operator-(const Vector4i& rhs) const;
  Vector4i& operator-=(const Vector4i& rhs);
  Vector4i operator*(int rhs) const;
  Vector4i& operator*=(int rhs);
};

template <typename Archive>
void serialize(Archive& archive, Vector4i& v)
{
  archive(v.x, v.y, v.z, v.w);
}

}  // namespace dl
