#pragma once

namespace dl
{
class Camera;

class CameraInspector
{
 public:
  bool open = true;

  CameraInspector(Camera& camera);
  void update();
  void toggle() { open = !open; }

 private:
  Camera& m_camera;
};
}  // namespace dl
