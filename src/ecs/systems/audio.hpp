#pragma once

#include <entt/entity/fwd.hpp>

namespace dl::audio
{
class AudioManager;
}  // namespace dl::audio

namespace dl
{
struct GameContext;

class AudioSystem
{
 public:
  AudioSystem(entt::registry& registry, GameContext& game_context);

  void update(entt::registry& registry);

 private:
  audio::AudioManager& m_audio_manager;

  void m_play_sound_effect(entt::registry& registry, entt::entity entity);
  void m_stop_sound_effect(entt::registry& registry, entt::entity entity);
};
}  // namespace dl
