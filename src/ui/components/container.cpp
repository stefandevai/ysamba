#include "./container.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "core/maths/vector.hpp"
#include "graphics/batch.hpp"

namespace dl::ui
{
Container::Container(const Vector2i& size, const std::string& color)
    : UIComponent(), quad(std::make_shared<Quad>(size.x, size.y, Color{color}))
{
  this->size = size;
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
