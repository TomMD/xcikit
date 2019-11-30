// VulkanPrimitives.h created on 2019-10-24, part of XCI toolkit
// Copyright 2019 Radek Brich
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef XCI_GRAPHICS_VULKAN_PRIMITIVES_H
#define XCI_GRAPHICS_VULKAN_PRIMITIVES_H

#include <xci/graphics/Primitives.h>

#include <vulkan/vulkan.h>

namespace xci::graphics {

class VulkanShader;
class VulkanRenderer;


class VulkanPrimitives : public Primitives {
public:
    explicit VulkanPrimitives(VulkanRenderer& renderer, VertexFormat format, PrimitiveType type);
    ~VulkanPrimitives() override;

    void reserve(size_t primitives, size_t vertices) override;

    void begin_primitive() override;
    void end_primitive() override;
    void add_vertex(ViewportCoords xy, float u, float v) override;
    void add_vertex(ViewportCoords xy, float u1, float v1, float u2, float v2) override;
    void add_vertex(ViewportCoords xy, Color c, float u, float v) override;
    void add_vertex(ViewportCoords xy, Color c, float u1, float v1, float u2, float v2) override;
    void clear() override;
    bool empty() const override;

    void set_shader(Shader& shader) override;
    void set_blend(BlendFunc func) override;

    void draw(View& view) override;

private:
    void create_pipeline();

private:
    VulkanRenderer& m_renderer;
    VulkanShader* m_shader = nullptr;
    VkPipelineLayout m_pipeline_layout {};
    VkPipeline m_pipeline {};
};


} // namespace xci::graphics

#endif // include guard
