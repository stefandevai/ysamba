#pragma once

namespace dl
{
struct Vector2
{
  double x;
  double y;
};

struct Vector2i
{
  int x;
  int y;

  Vector2i(const double x, const double y) : x(static_cast<int>(x)), y(static_cast<int>(y)) {}
};

struct Vector2f
{
  float x;
  float y;
};

struct Vector3
{
  double x;
  double y;
  double z;
};

struct Vector3i
{
  int x;
  int y;
  int z;

  Vector3i(const double x, const double y, const double z)
      : x(static_cast<int>(x)), y(static_cast<int>(y)), z(static_cast<int>(z))
  {
  }
};

struct Vector3f
{
  float x;
  float y;
  float z;
};
}  // namespace dl
