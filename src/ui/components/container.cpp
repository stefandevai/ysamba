#include "./container.hpp"

namespace dl::ui
{
Container::Container(const int width, const int height, const std::string& color)
    : UIComponent(ComponentType::Quad), quad(std::make_shared<Quad>(width, height, Color{color}))
{
}
}  // namespace dl::ui
