#include "./flex.hpp"

#include <spdlog/spdlog.h>

namespace dl::ui
{
Flex::Flex(UIContext& context, Params params) : UIComponent(context, "Flex"), direction(params.direction) {}

void Flex::update()
{
  if (children.empty() || !dirty)
  {
    return;
  }

  if (direction == FlexDirection::Row)
  {
    Vector2i anchor = Vector2i{};

    for (auto& child : children)
    {
      child->position.x = anchor.x;
      child->position.y = anchor.y;
      anchor.x += child->size.x;
    }
  }
  else if (direction == FlexDirection::Column)
  {
    Vector2i anchor = Vector2i{};

    for (auto& child : children)
    {
      child->position.x = anchor.x;
      child->position.y = anchor.y;
      anchor.y += child->size.y;
    }
  }
}

}  // namespace dl::ui
