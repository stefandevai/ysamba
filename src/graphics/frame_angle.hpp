#pragma once

enum class FrameAngle
{
  Parallel,
  Orthogonal,
};

// Defines in which face a sprite can be rendered.
// Bottom, Front, Back, Top, Left, Right are the faces of a cube.
// CenterHorizontal, CenterVertical render the sprite in the center of the cube area.
enum class RenderFace
{
  Bottom,
  Front,
  Back,
  Top,
  Left,
  Right,
  CenterHorizontal,
  CenterVertical,
};
