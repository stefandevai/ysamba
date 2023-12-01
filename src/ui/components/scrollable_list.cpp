#include "./scrollable_list.hpp"

#include <spdlog/spdlog.h>

#include "./button_list.hpp"
#include "./container.hpp"
#include "./scrollable.hpp"
#include "core/display.hpp"
#include "core/maths/vector.hpp"
#include "ui/style/list.hpp"

namespace dl::ui
{
ScrollableList::ScrollableList(const std::vector<std::string>& items,
                               const Vector2i& size,
                               const std::function<void(const int i)>& on_select)
    : UIComponent()
{
  this->size = size;

  ui::ListStyle style{{15, 15}, 5};

  /* const auto on_select = [](const int i) { */
  /*   spdlog::debug("SELECTED FROM LIST"); */

  /* }; */

  const auto list = std::make_shared<ui::ButtonList>(items, Vector2i{size.x, 45}, on_select, style);

  const auto scrollable = std::make_shared<ui::Scrollable>();
  scrollable->size = size;
  scrollable->children.push_back(list);

  const auto container = std::make_shared<ui::Container>(size, "#1b2420aa");
  container->children.push_back(scrollable);

  children.push_back(container);
}

}  // namespace dl::ui
