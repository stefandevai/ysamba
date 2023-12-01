#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include "./container.hpp"
#include "./list.hpp"
#include "./scrollable.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "ui/style/list.hpp"

namespace dl::ui
{
ScrollableList::ScrollableList(const std::vector<std::string>& items, const Vector2i& size) : UIComponent()
{
  this->size = size;

  ui::ListStyle style{{15, 0}, 25};
  const auto list = std::make_shared<ui::List>(items, style);

  const auto scrollable = std::make_shared<ui::Scrollable>();
  scrollable->size = size;
  scrollable->children.push_back(list);

  const auto container = std::make_shared<ui::Container>(size, "#1b2420aa");
  container->children.push_back(scrollable);

  children.push_back(container);
}

}  // namespace dl::ui
