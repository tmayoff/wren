#include "ui.hpp"

#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <wren/renderer.hpp>
#include <wren_vk/errors.hpp>

namespace editor::ui {

void CheckResult(VkResult res) {
  if (res != VK_SUCCESS) {
    throw std::runtime_error("Vulkan error");
  }
}

auto mouse_code_to_imgui(wren::MouseCode code) {
  switch (code) {
    case wren::MouseCode::Left:
      return ImGuiMouseButton_Left;
    case wren::MouseCode::Right:
      return ImGuiMouseButton_Right;
    case wren::MouseCode::Middle:
      return ImGuiMouseButton_Middle;
  }
}

auto init(std::shared_ptr<wren::Context> const& context)
    -> wren::expected<void> {
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Setup events
  context->event_dispatcher.on<wren::Event::MouseMoved>(
      [&io](auto const& e) {
        if (!e.relative) {
          io.AddMousePosEvent(e.x, e.y);
        } else {
          io.MouseDelta = {e.x, e.y};
        }
      });

  context->event_dispatcher.on<wren::Event::MouseButtonDown>(
      [&io](auto const& e) {
        io.AddMouseButtonEvent(mouse_code_to_imgui(e.button), true);
      });

  context->event_dispatcher.on<wren::Event::MouseButtonUp>(
      [&io](auto const& e) {
        io.AddMouseButtonEvent(mouse_code_to_imgui(e.button), false);
      });

  // context->event_dispatcher.on<wren::Event::MouseButtonUp>(
  //     [&io](auto const& e) {
  //       io.AddMouseButtonEvent(mouse_code_to_imgui(e.button),
  //       false);
  //     });

  ImGui_ImplSDL2_InitForVulkan(context->window.NativeHandle());

  auto const& graphics_context = context->graphics_context;

  ::vk::DescriptorPool pool;

  {
    std::array pool_sizes = {
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eSampler,
                                 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eCombinedImageSampler, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eSampledImage,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageImage,
                                 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eUniformTexelBuffer, 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eStorageTexelBuffer, 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eUniformBuffer,
                                 1000},
        ::vk::DescriptorPoolSize{::vk::DescriptorType::eStorageBuffer,
                                 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eUniformBufferDynamic, 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eStorageBufferDynamic, 1000},
        ::vk::DescriptorPoolSize{
            ::vk::DescriptorType::eInputAttachment, 1000},
    };

    ::vk::DescriptorPoolCreateInfo pool_info{
        ::vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
        1000 * pool_sizes.size(), pool_sizes};

    VK_TIE_RESULT(
        pool, graphics_context->Device().get().createDescriptorPool(
                  pool_info));
  }

  ImGui_ImplVulkan_InitInfo init_info{};
  init_info.Instance = graphics_context->Instance();
  init_info.PhysicalDevice = graphics_context->PhysicalDevice();
  init_info.Device = graphics_context->Device().get();
  init_info.Queue = graphics_context->Device().get_graphics_queue();
  init_info.QueueFamily =
      graphics_context->FindQueueFamilyIndices()->graphics_index;
  init_info.MinImageCount = 2;
  init_info.ImageCount =
      context->renderer->swapchain_images_views().size();
  init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
  init_info.RenderPass = context->renderer->get_graph()
                             .node_by_name("ui")
                             ->render_pass->get();
  init_info.DescriptorPool = pool;
  init_info.CheckVkResultFn = &CheckResult;

  if (!ImGui_ImplVulkan_Init(&init_info)) {
    spdlog::error("Failed to initialize vulkan for imgui");
  }

  spdlog::debug("Initialized ImGui");

  return {};
}

auto begin() -> void {
  ImGui_ImplVulkan_NewFrame();
  ImGui_ImplSDL2_NewFrame();
  ImGui::NewFrame();
}

auto end() -> void {}

void flush(::vk::CommandBuffer const& cmd) {
  ImGui::Render();
  ImDrawData* draw_data = ImGui::GetDrawData();

  // send draw data to command_buffer
  ImGui_ImplVulkan_RenderDrawData(draw_data, cmd);
}

}  // namespace editor::ui
