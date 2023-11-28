#include "./container.hpp"

#include "graphics/batch.hpp"

namespace dl::ui
{
Container::Container(const int width, const int height, const std::string& color)
    : UIComponent(ComponentType::Quad), quad(std::make_shared<Quad>(width, height, Color{color}))
{
}

void Container::render(Batch& batch) { batch.quad(quad, position.x, position.y, position.z); }
}  // namespace dl::ui
