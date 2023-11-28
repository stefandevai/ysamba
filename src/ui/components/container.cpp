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
  batch->quad(quad, position.x, position.y, position.z);

  if (position.x != 0 && position.y != 0)
  {
    auto translate = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z));
    batch->push_matrix(translate);
  }

  for (auto& child : children)
  {
    child->render(batch);
  }

  if (position.x != 0 && position.y != 0)
  {
    batch->pop_matrix();
  }
}
}  // namespace dl::ui
