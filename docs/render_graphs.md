# rendering

## design 

[RenderTarget](@ref wren::RenderTarget) is the main data structure for a render pass. They contain all the information to create and recreate an vk::Image and vk::ImageView.

When creating a pass you need to provide a [render resources object](@ref wren::PassResources) which contains the details for a particular pass: colour (+depth), shaders, etc. 

An example of how to craete a graph with a single pass with a colour target

```cpp
  wren::GraphBuilder builder(ctx);
  builder.add_pass("mesh",
          resouces, 
          [](wren::RenderPass& pass, vk::CommandBuffer& cmd){})
         .compile();
```

## plan

A render pass can be built with a shader and a render target. A shorthand can be used for specifying the swapchain as the render target, maybe by omitting the target.
When adding a new pass, by default the 'input' will be the last pass's target. A render pass needs to have a single output image. We should be able to swap out the currently
acquired swapchain image for the last render pass.


## 3d render pipeline

## notes

Depth is implemented with reverse depth (supposedly better accuracy). This means the depth values are 0 the farther away and 1 the closer they are. When creating the pipeline it also means the VK_COMPARE_OP must be GREATER(_OR_EQUAL).

## references
- [Pipeline caching](https://zeux.io/2019/07/17/serializing-pipeline-cache/)
- [https://ruby0x1.github.io/machinery_blog_archive/post/high-level-rendering-using-render-graphs/index.html](https://ruby0x1.github.io/machinery_blog_archive/post/high-level-rendering-using-render-graphs/index.html)
- [https://blog.traverseresearch.nl/render-graph-101-f42646255636](https://blog.traverseresearch.nl/render-graph-101-f42646255636)
- [https://logins.github.io/graphics/2021/05/31/RenderGraphs.html](https://logins.github.io/graphics/2021/05/31/RenderGraphs.html)
- [https://docs.unity3d.com/Packages/com.unity.render-pipelines.core@12.0/manual/render-graph-fundamentals.html](https://docs.unity3d.com/Packages/com.unity.render-pipelines.core@12.0/manual/render-graph-fundamentals.html)
- [https://github.com/bevyengine/bevy/discussions/9897](https://github.com/bevyengine/bevy/discussions/9897)
