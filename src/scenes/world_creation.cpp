#include "./world_creation.hpp"

#include <fmt/chrono.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <climits>
#include <entt/core/hashed_string.hpp>

#include "config.hpp"
#include "core/game_context.hpp"
#include "core/json.hpp"
#include "core/maths/random.hpp"
#include "core/maths/utils.hpp"
#include "core/maths/vector.hpp"
#include "core/scene_manager.hpp"
#include "core/serialization.hpp"
#include "core/utils.hpp"
#include "graphics/color.hpp"
#include "graphics/renderer/texture.hpp"
#include "ui/compositions/world_creation_panel.hpp"
#include "world/generators/island_generator.hpp"
#include "world/metadata.hpp"

namespace dl
{
WorldCreation::WorldCreation(GameContext& game_context) : Scene("world_creation", game_context) {}

void WorldCreation::load()
{
  world_size.x = config::world_creation::world_width;
  world_size.y = config::world_creation::world_height;
  world_size.z = config::world_creation::world_depth;

  m_panel = m_ui_manager.emplace<ui::WorldCreationPanel>(ui::WorldCreationPanel::Params{
      .on_save = [this]() { save(); },
  });

  m_generate_map();
  m_generate_world();
  m_create_map_representation();
  m_create_biome_representation();
  m_has_loaded = true;
}

void WorldCreation::update()
{
  using namespace entt::literals;

  if (!has_loaded())
  {
    return;
  }

  if (m_input_manager.is_context("world_creation"_hs))
  {
    m_update_input();
  }

  m_update_world_location_selection();

  m_camera.update(m_game_context.clock->delta);
  m_ui_manager.update();

  if (m_scene_state == SceneState::Pop)
  {
    m_game_context.scene_manager->pop_scene();
  }
}

void WorldCreation::render()
{
  if (!has_loaded())
  {
    return;
  }

  if (m_texture != nullptr)
  {
    m_renderer.ui_pass.batch.texture(*m_texture, 15, 15, 0);
  }
  if (m_biome_texture != nullptr)
  {
    m_renderer.ui_pass.batch.texture(*m_biome_texture, 15, 15, 1);
  }
  if (m_is_hovering_map)
  {
    m_renderer.ui_pass.batch.quad(m_location_selector_on_hover,
                                  m_location_selector_on_hover_position.x,
                                  m_location_selector_on_hover_position.y,
                                  2);
  }
  if (m_has_selected_location)
  {
    m_renderer.ui_pass.batch.quad(
        m_location_selector, m_location_selector_position.x, m_location_selector_position.y, 3);
  }

  m_ui_manager.render();
  m_renderer.render(m_camera);
}

bool WorldCreation::m_update_input()
{
  using namespace entt::literals;

  bool will_quit = false;

  if (m_input_manager.poll_action("quit"_hs))
  {
    m_scene_state = SceneState::Pop;
  }
  else if (m_input_manager.poll_action("generate_world"_hs))
  {
    m_generate_map();
    m_create_map_representation();
    m_create_biome_representation();
  }
  else if (m_input_manager.poll_action("save"_hs))
  {
    save();
  }
  else if (m_input_manager.poll_action("display_seed"_hs))
  {
    spdlog::info("SEED: {}", m_seed);
  }

  return will_quit;
}

void WorldCreation::save()
{
  bool is_valid = m_panel->validate() && m_selected_cell != Vector2i{-1, -1};

  if (!is_valid)
  {
    spdlog::debug("World parameters are invalid");
    return;
  }

  const auto now = std::chrono::system_clock::now();
  const auto now_seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);

  WorldMetadata metadata{
      .id = utils::generate_id(),
      .name = m_panel->get_name(),
      .seed = m_seed,
      .world_size = world_size,
      .created_at = now_seconds,
      .updated_at = now_seconds,
      .initial_position = m_selected_cell,
      .biome_map = std::move(m_biome_map),
      .height_map = std::move(m_height_map),
      .sea_distance_field = std::move(m_sea_distance_field),
  };

  serialization::save_world_metadata(metadata);
  serialization::save_world(m_world, metadata);

  spdlog::debug("World saved: {}", metadata.name);
  spdlog::debug("Seed: {}", metadata.seed);
  spdlog::debug("Id: {}", metadata.id);

  m_scene_state = SceneState::Pop;
}

void WorldCreation::m_generate_map()
{
  // Reload scene file
  JSON json{m_scene_path};

  bool use_random_seed = true;

  if (json.object.contains("random_seed"))
  {
    use_random_seed = json.object["random_seed"].get<bool>();
  }

  if (!use_random_seed && json.object.contains("seed"))
  {
    m_seed = json.object["seed"].get<int>();
  }
  else
  {
    m_seed = random::get_integer(1, INT_MAX);
  }

  auto generator = IslandGenerator(world_size);
  generator.generate(m_seed);

  m_height_map = std::move(generator.height_map);
  m_sea_distance_field = std::move(generator.sea_distance_field);
  m_biome_map = std::move(generator.biome_map);
}

void WorldCreation::m_generate_world()
{
  m_world.generate_societies();
}

void WorldCreation::m_create_map_representation()
{
  std::vector<unsigned char> pixel_data((world_size.x * world_size.y) * 4);

  for (auto i = 0; i < world_size.x; ++i)
  {
    for (auto j = 0; j < world_size.y; ++j)
    {
      auto map_value = std::floor(m_height_map[j * world_size.x + i] * 63.0f);
      map_value = std::clamp(map_value, 0.0f, 255.0f);

      if (map_value == 0.0f)
      {
        map_value = 100.0f;
      }

      pixel_data[j * world_size.x * 4 + i * 4] = map_value;
      pixel_data[j * world_size.x * 4 + i * 4 + 1] = map_value;
      pixel_data[j * world_size.x * 4 + i * 4 + 2] = map_value;
      pixel_data[j * world_size.x * 4 + i * 4 + 3] = 255;
    }
  }

  m_texture = std::make_unique<Texture>(
      m_game_context.display->wgpu_context.device, pixel_data.data(), Vector2i{world_size.x, world_size.y});

  // TODO: Find a better way to override texture size
  const auto& window_size = Display::get_window_size();
  const int texture_size = std::min(window_size.x, window_size.y) - 30;
  m_texture->size = Vector2i{texture_size, texture_size};

  // Set location selector size based on texels per cell
  m_has_selected_location = false;
  m_selected_cell = Vector2i{-1, -1};
  m_location_selector.w = (texture_size / world_size.x) * m_texels_per_cell;
  m_location_selector.h = (texture_size / world_size.y) * m_texels_per_cell;
  m_location_selector_on_hover.w = (texture_size / world_size.x) * m_texels_per_cell;
  m_location_selector_on_hover.h = (texture_size / world_size.y) * m_texels_per_cell;

  m_renderer.ui_pass.batch.clear_textures();
}

void WorldCreation::m_create_biome_representation()
{
  std::vector<unsigned char> pixel_data((world_size.x * world_size.y) * 4);
  Color color{0xFFFFFFFF};

  for (auto i = 0; i < world_size.x; ++i)
  {
    for (auto j = 0; j < world_size.y; ++j)
    {
      const auto biome_type = m_biome_map[j * world_size.x + i];

      switch (biome_type)
      {
      case BiomeType::Rainforest:
        color.set(0x13ae62FF);
        break;
      case BiomeType::TemperateForest:
        color.set(0x189c3fFF);
        break;
      case BiomeType::Beach:
        color.set(0xdcea8cFF);
        break;
      case BiomeType::Mangrove:
        color.set(0x1fcb93FF);
        break;
      case BiomeType::Sea:
        color.set(0x207991FF);
        break;
      case BiomeType::Lake:
        color.set(0x30dcb7FF);
        break;
      case BiomeType::Rocks:
        color.set(0x314b4bFF);
        break;
      case BiomeType::RockMountains:
        color.set(0x2a5143FF);
        break;
      case BiomeType::PineForestMountains:
        color.set(0x1c8563FF);
        break;
      case BiomeType::DryPlains:
        color.set(0x9bd191FF);
        break;
      case BiomeType::Meadows:
        color.set(0x5dc073FF);
        break;
      case BiomeType::Swamp:
        color.set(0x82cb30FF);
        break;
      default:
        color.set(0xFF00FFFF);
        break;
      }

      pixel_data[j * world_size.x * 4 + i * 4] = color.rgba_color.r;
      pixel_data[j * world_size.x * 4 + i * 4 + 1] = color.rgba_color.g;
      pixel_data[j * world_size.x * 4 + i * 4 + 2] = color.rgba_color.b;
      // pixel_data[j * world_size.x * 4 + i * 4 + 3] = 255;
      pixel_data[j * world_size.x * 4 + i * 4 + 3] = 127;
    }
  }

  m_biome_texture = std::make_unique<Texture>(
      m_game_context.display->wgpu_context.device, pixel_data.data(), Vector2i{world_size.x, world_size.y});

  // TODO: Find a better way to override texture size
  const auto& window_size = Display::get_window_size();
  const int texture_size = std::min(window_size.x, window_size.y) - 30;
  m_biome_texture->size = Vector2i{texture_size, texture_size};

  m_renderer.ui_pass.batch.clear_textures();
}

void WorldCreation::m_update_world_location_selection()
{
  if (m_texture == nullptr)
  {
    return;
  }

  const auto mouse_position = m_input_manager.get_mouse_position();

  if (!utils::point_aabb(mouse_position, panel_margin, m_texture->size))
  {
    m_is_hovering_map = false;
    return;
  }

  m_is_hovering_map = true;

  const double texel_width = m_texture->size.x / static_cast<double>(world_size.x);
  const double texel_height = m_texture->size.y / static_cast<double>(world_size.y);
  const int cell_position_x = (mouse_position.x - panel_margin.x) / texel_width;
  const int cell_position_y = (mouse_position.y - panel_margin.y) / texel_height;

  // m_location_selector_on_hover_position = ((mouse_position - panel_margin) / m_location_selector_on_hover.w) *
  // m_location_selector_on_hover.w + panel_margin;
  m_location_selector_on_hover_position
      = Vector2i{cell_position_x * texel_width + panel_margin.x, cell_position_y * texel_height + panel_margin.y};

  if (m_input_manager.has_clicked(InputManager::MouseButton::Left))
  {
    m_selected_cell = Vector2i{cell_position_x, cell_position_y};
    m_location_selector_position = m_location_selector_on_hover_position;
    m_has_selected_location = true;
  }
}

}  // namespace dl
