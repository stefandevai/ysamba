#include "./flex.hpp"

#include <spdlog/spdlog.h>

namespace dl::ui
{
Flex::Flex(UIContext& context, Params params)
    : UIComponent(context, "Flex"),
      direction(params.direction),
      horizontal_placement(params.horizontal_placement),
      vertical_placement(params.vertical_placement)
{
  this->size = std::move(params.size);
}

void Flex::update()
{
  if (children.empty() || !dirty)
  {
    return;
  }

  // Align children according to the direction
  if (direction == FlexDirection::Row)
  {
    // Handle horizontal placement
    if (horizontal_placement == FlexPlacement::Start)
    {
      int anchor_x = 0;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x;
      }
    }
    else if (horizontal_placement == FlexPlacement::End)
    {
      int anchor_x = size.x;

      for (auto it = children.rbegin(); it != children.rend(); ++it)
      {
        auto& child = *it;
        anchor_x -= child->size.x;
        child->position.x = anchor_x;
      }
    }
    else if (horizontal_placement == FlexPlacement::Center)
    {
      int children_width = 0;

      for (auto& child : children)
      {
        children_width += child->size.x;
      }

      int anchor_x = (size.x - children_width) / 2;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x;
      }
    }
    else if (horizontal_placement == FlexPlacement::SpaceBetween && children.size() > 1)
    {
      int children_width = 0;

      for (auto& child : children)
      {
        children_width += child->size.x;
      }

      int spacing = (size.x - children_width) / (children.size() - 1);

      int anchor_x = 0;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x + spacing;
      }
    }
    else if (horizontal_placement == FlexPlacement::SpaceEvenly)
    {
      int children_width = 0;

      for (auto& child : children)
      {
        children_width += child->size.x;
      }

      int spacing = (size.x - children_width) / (children.size() + 1);

      int anchor_x = spacing;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x + spacing;
      }
    }

    // Handle vertical placement
    if (vertical_placement == FlexPlacement::End)
    {
      for (auto& child : children)
      {
        child->position.y = size.y - child->size.y;
      }
    }
    else if (vertical_placement == FlexPlacement::Center)
    {
      for (auto& child : children)
      {
        child->position.y = (size.y - child->size.y) / 2;
      }
    }
    else if (vertical_placement == FlexPlacement::SpaceEvenly)
    {
      int children_height = 0;

      for (auto& child : children)
      {
        children_height += child->size.y;
      }

      const int spacing = (size.y - children_height) / (children.size() + 1);
      int anchor_y = spacing;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y + spacing;
      }
    }
    else if (vertical_placement == FlexPlacement::SpaceBetween && children.size() > 1)
    {
      int children_height = 0;

      for (auto& child : children)
      {
        children_height += child->size.y;
      }

      const int spacing = (size.y - children_height) / (children.size() - 1);
      int anchor_y = 0;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y + spacing;
      }
    }
  }
  else if (direction == FlexDirection::Column)
  {
    // Handle vertical placement
    if (vertical_placement == FlexPlacement::Start)
    {
      int anchor_y = 0;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y;
      }
    }
    else if (vertical_placement == FlexPlacement::End)
    {
      int anchor_y = size.y;

      for (auto it = children.rbegin(); it != children.rend(); ++it)
      {
        auto& child = *it;
        anchor_y -= child->size.y;
        child->position.y = anchor_y;
      }
    }
    else if (vertical_placement == FlexPlacement::Center)
    {
      int children_height = 0;

      for (auto& child : children)
      {
        children_height += child->size.y;
      }

      int anchor_y = (size.y - children_height) / 2;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y;
      }
    }
    else if (vertical_placement == FlexPlacement::SpaceBetween && children.size() > 1)
    {
      int children_height = 0;

      for (auto& child : children)
      {
        children_height += child->size.y;
      }

      int spacing = (size.y - children_height) / (children.size() - 1);

      int anchor_y = 0;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y + spacing;
      }
    }
    else if (vertical_placement == FlexPlacement::SpaceEvenly)
    {
      int children_height = 0;

      for (auto& child : children)
      {
        children_height += child->size.y;
      }

      int spacing = (size.y - children_height) / (children.size() + 1);

      int anchor_y = spacing;

      for (auto& child : children)
      {
        child->position.y = anchor_y;
        anchor_y += child->size.y + spacing;
      }
    }

    // Handle horizontal placement
    if (horizontal_placement == FlexPlacement::End)
    {
      for (auto& child : children)
      {
        child->position.x = size.x - child->size.x;
      }
    }
    else if (horizontal_placement == FlexPlacement::Center)
    {
      for (auto& child : children)
      {
        child->position.x = (size.x - child->size.x) / 2;
      }
    }
    else if (horizontal_placement == FlexPlacement::SpaceEvenly)
    {
      int children_width = 0;

      for (auto& child : children)
      {
        children_width += child->size.x;
      }

      const int spacing = (size.x - children_width) / (children.size() + 1);
      int anchor_x = spacing;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x + spacing;
      }
    }
    else if (horizontal_placement == FlexPlacement::SpaceBetween && children.size() > 1)
    {
      int children_width = 0;

      for (auto& child : children)
      {
        children_width += child->size.x;
      }

      const int spacing = (size.x - children_width) / (children.size() - 1);
      int anchor_x = 0;

      for (auto& child : children)
      {
        child->position.x = anchor_x;
        anchor_x += child->size.x + spacing;
      }
    }
  }
}

}  // namespace dl::ui
