#include "./debug.hpp"

#include <glad/glad.h>

#include "./lib/imgui_impl_opengl3.h"
#include "./lib/imgui_impl_sdl2.h"
#include "SDL.h"
#include "imgui.h"

namespace dl
{
std::unique_ptr<Debug> Debug::m_instance = nullptr;

Debug::~Debug()
{
  if (!m_has_initialized)
  {
    return;
  }

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
}

Debug& Debug::get_instance()
{
  if (m_instance == nullptr)
  {
    m_instance = std::make_unique<Debug>();
  }

  return *m_instance;
}

void Debug::init(SDL_Window* window, SDL_GLContext& context)
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

void Debug::process_event(SDL_Event* event)
{
  if (!m_has_initialized)
  {
    return;
  }

  ImGui_ImplSDL2_ProcessEvent(event);
}

bool show_demo_window = false;

void Debug::update()
{
  if (!m_has_initialized)
  {
    return;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();

  if (show_demo_window)
  {
    ImGui::ShowDemoWindow(&show_demo_window);
  }

  if (m_camera_editor != nullptr)
  {
    m_camera_editor->update();
  }
}

void Debug::render()
{
  if (!m_has_initialized)
  {
    return;
  }

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Debug::init_camera_editor(Camera& camera) { m_camera_editor = std::make_unique<CameraEditor>(camera); }

}  // namespace dl
