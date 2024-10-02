#include "editor.hpp"

#include <imgui.h>
#include <imgui_impl_vulkan.h>
#include <imgui_internal.h>

#include <tracy/Tracy.hpp>
#include <wren/application.hpp>
#include <wren/context.hpp>
#include <wren/renderer.hpp>
#include <wren/shaders/mesh.hpp>

#include "ui.hpp"

auto Editor::New(std::shared_ptr<wren::Application> const &app)
    -> wren::expected<std::shared_ptr<Editor>> {
  auto editor = std::make_shared<Editor>(app->context());

  TRY_RESULT(auto graph,
             editor->build_ui_render_graph(app->context()));
  app->context()->renderer->set_graph_builder(graph);

  editor::ui::init(app->context());

  vk::SamplerCreateInfo sampler_info{};

  auto t =
      app->context()->graphics_context->Device().get().createSampler(
          sampler_info);
  editor->texture_sampler = t.value;

  editor->dset.resize(1);
  // for (uint32_t i = 0; i < 1; i++)
  editor->dset[0] = ImGui_ImplVulkan_AddTexture(
      editor->texture_sampler, editor->scene_view,
      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  app->add_callback_to_phase(wren::CallbackPhase::Update,
                             [editor]() { editor->on_update(); });

  return editor;
}

void Editor::on_update() {
  ZoneScoped;  // NOLINT
  editor::ui::begin();

  static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_None;

  ImGuiViewport const *viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0F);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0F);

  window_flags |=
      ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
      ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
      ImGuiWindowFlags_NoBringToFrontOnFocus |
      ImGuiWindowFlags_NoNavFocus;

  ImGui::Begin("Editor", nullptr, window_flags);
  ImGui::PopStyleVar(2);

  ImGuiIO &io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("Editor");
    ImGui::DockSpace(dockspace_id, ImVec2{0.0F, 0.0F},
                     dockspace_flags);

    static bool first_time = true;
    if (first_time) {
      first_time = false;

      ImGui::DockBuilderRemoveNode(dockspace_id);
      ImGui::DockBuilderAddNode(
          dockspace_id,
          dockspace_flags | ImGuiDockNodeFlags_DockSpace);
      ImGui::DockBuilderSetNodeSize(dockspace_id, viewport->Size);

      auto const dockspace_id_bottom = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Down, 0.25, nullptr, &dockspace_id);

      auto const dockspace_id_right =
          ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right,
                                      0.25F, nullptr, &dockspace_id);
      auto const dockspace_id_left = ImGui::DockBuilderSplitNode(
          dockspace_id, ImGuiDir_Left, 0.25F, nullptr, &dockspace_id);

      ImGui::DockBuilderDockWindow("Inspector", dockspace_id_right);
      ImGui::DockBuilderDockWindow("Viewer", dockspace_id);
      ImGui::DockBuilderDockWindow("Scene", dockspace_id_left);
      ImGui::DockBuilderDockWindow("Filesystem", dockspace_id_bottom);

      ImGui::DockBuilderFinish(dockspace_id_bottom);
    }
  }

  ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2{500, 100});
  ImGui::Begin("Scene");
  ImGui::End();

  ImGui::Begin("Viewer");
  auto curr_size = ImGui::GetContentRegionAvail();
  auto curr_size_vec = wren::math::vec2f{curr_size.x, curr_size.y};
  if (curr_size_vec != last_scene_size) {
    // TODO resize target
    last_scene_size = curr_size_vec;
  }

  ImGui::Image(dset[0], {last_scene_size.x(), last_scene_size.y()});
  ImGui::End();

  ImGui::Begin("Inspector");
  ImGui::End();

  ImGui::Begin("Filesystem");
  ImGui::End();

  ImGui::End();
  ImGui::PopStyleVar();

  editor::ui::end();
}

auto Editor::build_ui_render_graph(
    std::shared_ptr<wren::Context> const &ctx)
    -> wren::expected<wren::GraphBuilder> {
  wren::GraphBuilder builder(ctx);

  auto target = std::make_shared<wren::RenderTarget>(
      ::vk::Extent2D{512, 512}, ::vk::Format::eB8G8R8A8Srgb,
      ::vk::SampleCountFlagBits::e1, nullptr,
      ::vk::ImageUsageFlagBits::eColorAttachment |
          ::vk::ImageUsageFlagBits::eSampled);

  ::vk::ImageCreateInfo image_info(
      {}, ::vk::ImageType::e2D, target->format,
      ::vk::Extent3D(target->size.width, target->size.height, 1), 1,
      1);
  image_info.setUsage(target->image_usage);
  image_info.setSharingMode(::vk::SharingMode::eExclusive);

  VmaAllocationCreateInfo alloc_info{};
  alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
  auto const &allocator = ctx->graphics_context->allocator();
  auto info = static_cast<VkImageCreateInfo>(image_info);
  vmaCreateImage(allocator, &info, &alloc_info, &scene_image,
                 &scene_alloc_, nullptr);

  // transition image
  TRY_RESULT(ctx->renderer->submit_command_buffer(
      [this](::vk::CommandBuffer const &cmd_buf) {
        vk::ImageMemoryBarrier barrier(
            ::vk::AccessFlagBits::eTransferRead,
            ::vk::AccessFlagBits::eMemoryRead,
            ::vk::ImageLayout::eUndefined,
            ::vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED,
            scene_image,
            vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor,
                                      0, 1, 0, 1));

        cmd_buf.pipelineBarrier(
            ::vk::PipelineStageFlagBits::eTransfer,
            ::vk::PipelineStageFlagBits::eTransfer,
            vk::DependencyFlags(), {}, {}, barrier);
      }));

  ::vk::ImageViewCreateInfo image_view_info(
      {}, scene_image, ::vk::ImageViewType::e2D, target->format, {},
      ::vk::ImageSubresourceRange(::vk::ImageAspectFlagBits::eColor,
                                  0, 1, 0, 1));
  VK_TIE_RESULT(scene_view,
                ctx->graphics_context->Device().get().createImageView(
                    image_view_info));
  target->image_view = scene_view;
  target->final_layout = ::vk::ImageLayout::eShaderReadOnlyOptimal;

  TRY_RESULT(auto mesh_shader,
             wren::vk::Shader::Create(
                 ctx->graphics_context->Device().get(),
                 wren::shaders::MESH_VERT_SHADER.data(),
                 wren::shaders::MESH_FRAG_SHADER.data()));

  mesh = wren::Mesh(ctx->graphics_context->Device(),
                    ctx->graphics_context->allocator());
  mesh.shader(mesh_shader);

  builder
      .add_pass("mesh",
                {
                    {
                        {"mesh", mesh_shader},
                    },
                    "scene_viewer",
                    target,
                },
                [this, ctx](wren::RenderPass &pass,
                            ::vk::CommandBuffer &cmd) {
                  pass.bind_pipeline("mesh");
                  mesh.bind(cmd);
                  mesh.draw(cmd);
                })
      .add_pass("ui", {{}, "swapchain_target"},
                [](wren::RenderPass &pass, ::vk::CommandBuffer &cmd) {
                  editor::ui::flush(cmd);
                });

  return builder;
}
