#pragma once

#include "./component.hpp"

namespace dl::ui
{
enum class FlexDirection
{
  Row,
  Column,
};

enum class FlexPlacement
{
  Start,
  Center,
  End,
  SpaceEvenly,
  SpaceBetween,
};

class Flex : public UIComponent
{
 public:
  struct Params
  {
    Vector2i size;
    FlexDirection direction = FlexDirection::Row;
    FlexPlacement horizontal_placement = FlexPlacement::Start;
    FlexPlacement vertical_placement = FlexPlacement::Start;
  };

  FlexDirection direction{};
  FlexPlacement horizontal_placement{};
  FlexPlacement vertical_placement{};

  Flex(UIContext& context, Params params);

  void update() override;
};

}  // namespace dl::ui
