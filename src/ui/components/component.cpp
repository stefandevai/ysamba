#include "./component.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/batch.hpp"

namespace dl::ui
{
void UIComponent::update(const std::shared_ptr<Batch> batch)
{
  if (position.x != 0 && position.y != 0)
  {
    auto translate = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, position.z));
    batch->push_matrix(translate);
  }

  for (auto& child : children)
  {
    child->update(batch);
  }

  if (position.x != 0 && position.y != 0)
  {
    batch->pop_matrix();
  }
}

void UIComponent::render(const std::shared_ptr<Batch> batch)
{
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
