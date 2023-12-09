#pragma once

namespace dl
{
struct Selectable
{
  bool selected = false;
};

template <typename Archive>
void serialize(Archive& archive, Selectable& selectable)
{
  archive(selectable.selected);
}
}  // namespace dl
