#pragma once

#include <entt/signal/emitter.hpp>

namespace dl
{
struct EventEmitter : public entt::emitter<EventEmitter>
{
};
}  // namespace dl
