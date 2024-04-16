#include "./flex.hpp"

#include <spdlog/spdlog.h>

namespace dl::ui
{
Flex::Flex(UIContext& context, Params params) : UIComponent(context, "Flex"), direction(params.direction) {}

void Flex::update()
{
  if (children.empty())
  {
    return;
  }

  // compute_bounding_box();

  if (direction == FlexDirection::Row)
  {
    Vector2i anchor = absolute_position.xy();

    for (auto& child : children)
    {
      child->position.x = anchor.x;
      child->position.y = anchor.y;
      anchor.x += child->size.x;
    }
  }
}

}  // namespace dl::ui
