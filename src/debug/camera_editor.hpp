#pragma once

namespace dl
{
class Camera;

class CameraEditor
{
 public:
  CameraEditor(Camera& camera);
  void update();

 private:
  Camera& m_camera;
  bool m_open = true;
};
}  // namespace dl
