#include "./audio.hpp"

#include <spdlog/spdlog.h>

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>

#include "audio/audio_manager.hpp"
#include "core/game_context.hpp"
#include "ecs/components/position.hpp"
#include "ecs/components/sound_effect.hpp"

namespace dl
{
AudioSystem::AudioSystem(entt::registry& registry, GameContext& game_context)
    : m_audio_manager(*game_context.audio_manager)
{
  registry.on_construct<SoundEffect>().connect<&AudioSystem::m_play_sound_effect>(this);
  registry.on_destroy<SoundEffect>().connect<&AudioSystem::m_stop_sound_effect>(this);
}

void AudioSystem::update(entt::registry& registry)
{
  auto view = registry.view<SoundEffect>();

  for (const auto entity : view)
  {
    auto& sound_effect = view.get<SoundEffect>(entity);

    if (sound_effect.source == nullptr)
    {
      registry.destroy(entity);
      continue;
    }

    if (sound_effect.source->state == audio::SoundState::Stopped)
    {
      registry.destroy(entity);
    }
  }
}

void AudioSystem::m_play_sound_effect(entt::registry& registry, entt::entity entity)
{
  auto& sound_effect = registry.get<SoundEffect>(entity);
  m_audio_manager.sound_effect(sound_effect.id, sound_effect.loop, sound_effect.fade);
}

void AudioSystem::m_stop_sound_effect(entt::registry& registry, entt::entity entity)
{
  auto& sound_effect = registry.get<SoundEffect>(entity);

  if (sound_effect.source == nullptr)
  {
    return;
  }

  m_audio_manager.stop(sound_effect.source);
}
}  // namespace dl
