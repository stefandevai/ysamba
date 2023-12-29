#include "./debug_tools.hpp"

#include <glad/glad.h>

#include "./lib/imgui_impl_opengl3.h"
#include "./lib/imgui_impl_sdl2.h"
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

  ImGui_ImplOpenGL3_Shutdown();
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

void DebugTools::init(SDL_Window* window, SDL_GLContext& context)
{
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  ImGui::StyleColorsDark();

  ImGui_ImplSDL2_InitForOpenGL(window, context);
  ImGui_ImplOpenGL3_Init("#version 150");

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

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  m_update_menu_bar();

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
}

void DebugTools::render()
{
  if (!m_has_initialized || !open)
  {
    return;
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

}  // namespace dl
