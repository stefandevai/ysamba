#pragma once

#include "./component.hpp"

namespace dl::ui
{
enum class FlexDirection
{
  Row,
  Column,
};

class Flex : public UIComponent
{
 public:
  struct Params
  {
    FlexDirection direction = FlexDirection::Row;
  };

  FlexDirection direction = FlexDirection::Row;

  Flex(UIContext& context, Params params);

  void update() override;
};

}  // namespace dl::ui
