#include "./container.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/batch.hpp"

namespace dl::ui
{
Container::Container(const int width, const int height, const std::string& color)
    : UIComponent(ComponentType::Quad), quad(std::make_shared<Quad>(width, height, Color{color}))
{
}

void Container::render(const std::shared_ptr<Batch> batch)
{
  batch->quad(quad, absolute_position.x, absolute_position.y, absolute_position.z);

  for (auto& child : children)
  {
    child->render(batch);
  }
}
}  // namespace dl::ui
