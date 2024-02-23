#include "./debug_tools.hpp"

#include <spdlog/spdlog.h>
#include <webgpu/wgpu.h>

#include "./lib/imgui_impl_sdl2.h"
#include "./lib/imgui_impl_wgpu.h"
#include "SDL.h"
#include "imgui.h"

namespace dl
{
std::unique_ptr<DebugTools> DebugTools::m_instance = nullptr;

DebugTools::~DebugTools()
{
  if (!m_has_initialized)
  {
    return;
  }

  ImGui_ImplWGPU_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

DebugTools& DebugTools::get_instance()
{
  if (m_instance == nullptr)
  {
    m_instance = std::make_unique<DebugTools>();
  }

  return *m_instance;
}

void DebugTools::init(SDL_Window* window, WGPUDeviceImpl* device)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  ImGui_ImplWGPU_InitInfo init_info{};
  init_info.Device = device;
  init_info.RenderTargetFormat = WGPUTextureFormat_BGRA8Unorm;
  // init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;

  ImGui_ImplSDL2_InitForOther(window);
  ImGui_ImplWGPU_Init(&init_info);

  m_has_initialized = true;
}

void DebugTools::process_event(SDL_Event* event)
{
  if (!m_has_initialized || !open)
  {
    return;
  }

  ImGui_ImplSDL2_ProcessEvent(event);
}

bool show_demo_window = false;

void DebugTools::update()
{
  if (!m_has_initialized || !open)
  {
    return;
  }

  ImGui_ImplWGPU_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  // m_update_menu_bar();

  if (show_demo_window)
  {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  if (m_general_info != nullptr)
  {
    m_general_info->update();
  }
  if (m_camera_inspector != nullptr)
  {
    m_camera_inspector->update();
  }
  if (m_render_editor != nullptr)
  {
    m_render_editor->update();
  }
  if (m_chunk_debugger != nullptr)
  {
    m_chunk_debugger->update();
  }
  if (m_world_generation != nullptr)
  {
    m_world_generation->update();
  }
}

void DebugTools::render(WGPURenderPassEncoderImpl* render_pass)
{
  if (!m_has_initialized || !open)
  {
    return;
  }

  ImGui::Render();
  ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), render_pass);
}

void DebugTools::m_update_menu_bar()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("View"))
    {
      if (m_general_info != nullptr)
      {
        ImGui::MenuItem("General Info", NULL, &m_general_info->open);
      }
      if (m_camera_inspector != nullptr)
      {
        ImGui::MenuItem("Camera Inspector", NULL, &m_camera_inspector->open);
      }
      if (m_camera_inspector != nullptr)
      {
        ImGui::MenuItem("Chunk Debugger", NULL, &m_chunk_debugger->open);
      }

      ImGui::MenuItem("Demo Window", NULL, &show_demo_window);

      ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
  }
}

void DebugTools::init_general_info(GameContext& context) { m_general_info = std::make_unique<GeneralInfo>(context); }

void DebugTools::init_camera_inspector(Camera& camera)
{
  m_camera_inspector = std::make_unique<CameraInspector>(camera);
}

void DebugTools::init_render_editor(RenderSystem& render) { m_render_editor = std::make_unique<RenderEditor>(render); }

void DebugTools::init_chunk_debugger(Gameplay& gameplay)
{
  m_chunk_debugger = std::make_unique<ChunkDebugger>(gameplay);
}

void DebugTools::init_world_generation(ChunkManager& chunk_manager)
{
  m_world_generation = std::make_unique<WorldGeneration>(chunk_manager);
}

}  // namespace dl
