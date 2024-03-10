#include "./vector.hpp"

namespace dl
{
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
bool Vector2::operator==(const Vector2& rhs) const { return x == rhs.x && y == rhs.y; }

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

bool Vector2i::operator==(const Vector2i& rhs) const { return x == rhs.x && y == rhs.y; }

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

Vector3& Vector3::operator+=(const Vector3& rhs)
{
  this->x += rhs.x;
  this->y += rhs.y;
  this->z += rhs.z;
  return *this;
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

Vector4i::Vector4i(const Vector4i& other)
{
  x = other.x;
  y = other.y;
  z = other.z;
  w = other.w;
}

bool Vector4i::operator==(const Vector4i& rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }

Vector4i& Vector4i::operator=(const Vector4i& rhs)
{
  x = rhs.x;
  y = rhs.y;
  z = rhs.z;
  w = rhs.w;
  return *this;
}

}  // namespace dl
