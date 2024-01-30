#include "./general_info.hpp"

#ifdef __APPLE__
#include <mach/mach.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif
#include <spdlog/spdlog.h>
#include <stdlib.h>
#include <sys/types.h>

#include <chrono>

#include "core/game_context.hpp"
#include "definitions.hpp"
#include "imgui.h"
#include "implot.h"

namespace
{
struct ScrollingBuffer
{
  int max_size;
  int offset;

  ImVector<ImVec2> data;

  ScrollingBuffer(int max_size = 2000)
  {
    this->max_size = max_size;
    offset = 0;
    data.reserve(max_size);
  }

  void add_point(float x, float y)
  {
    if (data.size() < max_size)
    {
      data.push_back(ImVec2(x, y));
    }
    else
    {
      data[offset] = ImVec2(x, y);
      offset = (offset + 1) % max_size;
    }
  }

  void erase()
  {
    if (data.size() > 0)
    {
      data.shrink(0);
      offset = 0;
    }
  }
};

#ifdef __APPLE__
float get_cpu_usage_macos()
{
  kern_return_t kr;
  pid_t pid = getpid();
  task_t port;

  task_for_pid(mach_task_self(), pid, &port);

  task_info_data_t tinfo;

  mach_msg_type_number_t task_info_count;

  task_info_count = TASK_INFO_MAX;

  kr = task_info(port, TASK_BASIC_INFO, (task_info_t)tinfo, &task_info_count);

  if (kr != KERN_SUCCESS)
  {
    spdlog::debug("Could not get cpu usage");
    return 0.0f;
  }

  task_basic_info_t basic_info;
  thread_array_t thread_list;
  mach_msg_type_number_t thread_count;

  thread_info_data_t thinfo;
  mach_msg_type_number_t thread_info_count;

  thread_basic_info_t basic_info_th;
  uint32_t stat_thread = 0;  // Mach threads

  basic_info = (task_basic_info_t)tinfo;

  // get threads in the task
  kr = task_threads(port, &thread_list, &thread_count);

  if (kr != KERN_SUCCESS)
  {
    spdlog::debug("Could not get cpu usage");
    return 0.0f;
  }

  if (thread_count > 0)
  {
    stat_thread += thread_count;
  }

  long tot_cpu = 0;
  size_t j;

  for (j = 0; j < thread_count; j++)
  {
    thread_info_count = THREAD_INFO_MAX;
    kr = thread_info(thread_list[j], THREAD_BASIC_INFO, (thread_info_t)thinfo, &thread_info_count);
    if (kr != KERN_SUCCESS)
    {
      continue;
    }

    basic_info_th = (thread_basic_info_t)thinfo;

    if (!(basic_info_th->flags & TH_FLAGS_IDLE))
    {
      tot_cpu = tot_cpu + basic_info_th->cpu_usage;
    }
  }

  return tot_cpu / 1000.0f;
}

float get_ram_usage_macos()
{
  struct task_basic_info t_info;

  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;

  if (KERN_SUCCESS != task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info, &t_info_count))
  {
    return 0.0f;
  }

  return t_info.resident_size / 1000000.0f;
}
#endif
}  // namespace

namespace dl
{
GeneralInfo::GeneralInfo(GameContext& context) : m_game_context(context)
{
  ImPlot::CreateContext();
  ImPlotStyle& style = ImPlot::GetStyle();
  ImVec4* colors = style.Colors;
  colors[ImPlotCol_FrameBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.0f);
  colors[ImPlotCol_Line] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
  colors[ImPlotCol_PlotBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImPlotCol_PlotBorder] = ImVec4(0.80f, 0.80f, 0.80f, 0.50f);
  colors[ImPlotCol_AxisGrid] = ImVec4(0.70f, 0.70f, 0.70f, 0.30f);
  colors[ImPlotCol_LegendBorder] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
  colors[ImPlotCol_LegendBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.40f);
}

GeneralInfo::~GeneralInfo() { ImPlot::DestroyContext(); }

void GeneralInfo::update()
{
  if (!open)
  {
    return;
  }

  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                  ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
                                  ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

  const float padding = 15.0f;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImVec2 work_pos = viewport->WorkPos;
  ImVec2 window_pos, window_pos_pivot;
  window_pos.x = work_pos.x + padding;
  window_pos.y = work_pos.y + padding;
  window_pos_pivot.x = 0.0f;
  window_pos_pivot.y = 0.0f;
  ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
  ImGui::SetNextWindowBgAlpha(0.4f);

  if (ImGui::Begin("General Info", &open, window_flags))
  {
    ImGui::Text("FPS: %.1f", static_cast<float>(1.0 / m_game_context.clock->delta));
    ImGui::Text("MS: %.3f", static_cast<float>(m_game_context.clock->delta));

#ifdef DL_HAS_SUPPORTED_PLATFORM_FOR_USAGE
    m_render_usage_info();
#endif
  }

  /* ImPlot::ShowDemoWindow(); */

  ImGui::End();
}

#ifdef DL_HAS_SUPPORTED_PLATFORM_FOR_USAGE
void GeneralInfo::m_render_usage_info()
{
  static float ram_usage = 0.0f;
  static float cpu_usage = 0.0f;

  static auto last = std::chrono::steady_clock::now();
  const auto now = std::chrono::steady_clock::now();
  const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last).count();

  if (elapsed > 500)
  {
#ifdef __APPLE__
    cpu_usage = get_cpu_usage_macos();
    ram_usage = get_ram_usage_macos();
#endif
    last = now;
  }

  if (ram_usage > 0.0f)
  {
    ImGui::Text("RAM: %.2fMB", ram_usage);
  }

  if (cpu_usage > 0.0f)
  {
    static ScrollingBuffer sdata2;
    static float t = 0;
    t += ImGui::GetIO().DeltaTime;
    sdata2.add_point(t, cpu_usage);

    static float history = 10.0f;
    static ImPlotAxisFlags flags = ImPlotAxisFlags_NoHighlight | ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoTickMarks |
                                   ImPlotAxisFlags_NoTickLabels;

    ImPlot::PushStyleVar(ImPlotStyleVar_MinorAlpha, 1.00f);
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
    ImPlot::PushStyleVar(ImPlotStyleVar_LegendPadding, ImVec2(0, 0));
    if (ImPlot::BeginPlot("##CPU",
                          ImVec2(161, 100),
                          ImPlotFlags_NoTitle | ImPlotFlags_NoMouseText | ImPlotFlags_NoInputs | ImPlotFlags_NoMenus |
                              ImPlotFlags_NoBoxSelect))
    {
      ImPlot::SetupLegend(ImPlotLocation_South | ImPlotLocation_West, 0);
      ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
      ImPlot::SetupAxisLimits(ImAxis_X1, t - history, t, ImGuiCond_Always);
      ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 1);
      ImPlot::PlotLine(
          "CPU (%)", &sdata2.data[0].x, &sdata2.data[0].y, sdata2.data.size(), 0, sdata2.offset, 2 * sizeof(float));
      ImPlot::EndPlot();
    }
    ImPlot::PopStyleVar(3);
  }
}
#endif

}  // namespace dl
