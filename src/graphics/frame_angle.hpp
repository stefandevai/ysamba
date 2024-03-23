#pragma once

// enum class FrameAngle
// {
//   Parallel,
//   Orthogonal,
// };

// Defines in which face a sprite can be rendered.
// Top, Front, Back, Bottom, Left, Right are the faces of a cube.
// CenterHorizontal, CenterVertical render the sprite in the center of the cube area.
enum RenderFace
{
  DL_RENDER_FACE_TOP,
  DL_RENDER_FACE_FRONT,
  DL_RENDER_FACE_BACK,
  DL_RENDER_FACE_BOTTOM,
  DL_RENDER_FACE_LEFT,
  DL_RENDER_FACE_RIGHT,
  DL_RENDER_FACE_CENTER_HORIZONTAL,
  DL_RENDER_FACE_CENTER_VERTICAL,
};
