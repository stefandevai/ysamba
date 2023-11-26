#include "./scene.hpp"

#include <SDL.h>

#include "core/game_context.hpp"
#include "core/input_manager.hpp"

namespace dl
{
Scene::Scene(const std::string& scene_key, GameContext& game_context)
    : m_scene_key(scene_key), m_scene_dir("scenes/" + scene_key + "/"), m_game_context(game_context)
{
}

Scene::~Scene() {}

void Scene::load() { m_lua.load(m_scene_dir / "main.lua"); }
}  // namespace dl
