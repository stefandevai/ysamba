#pragma once

#include "./component.hpp"

namespace dl::ui
{
class Button : public UIComponent
{
 public:
  std::function<void()> on_click{};

  Button(const std::string& text, const Vector3i& position, const Vector2i& size = {100, 50});

  void update(const std::shared_ptr<Batch> batch);

 private:
  Vector2i m_size;
};

}  // namespace dl::ui
