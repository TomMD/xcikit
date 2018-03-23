// View.h created on 2018-03-04, part of XCI toolkit

#ifndef XCI_GRAPHICS_VIEW_H
#define XCI_GRAPHICS_VIEW_H

#include <xci/util/geometry.h>

#include <memory>

namespace xci {
namespace graphics {

using xci::util::Vec2u;
using xci::util::Vec2f;


class View {
public:
    explicit View(Vec2u pixel_size);
    ~View();
    View(View&&) noexcept;
    View& operator=(View&&) noexcept;

    void resize(Vec2u pixel_size);

    // Size of the view in display units.
    // These units are similar to base OpenGL coordinates, but with aspect
    // ratio correction. Center is {0,0}, bottom-left might be {-1.333, -1},
    // top-right might be {1.333, 1} (depending on aspect ratio). Total size
    // in one of the dimensions should always equal 2.0.
    // Eg: {2.666, 2.0} for 800x600 (4/3 aspect ratio)
    Vec2f size() const;

    // Size of the view in framebuffer pixels.
    // This is used for pixel-perfect font rendering.
    Vec2u pixel_size() const;

    // Size of 1x1 pixel square in display units.
    Vec2f pixel_ratio() const {
        auto p = pixel_size();
        auto u = size();
        return {u.x / p.x, u.y / p.y};
    }

    // Visual debugging
    using DebugFlags = unsigned int;
    enum class Debug: DebugFlags {
        GlyphBBox       = 1u << 0u,
        WordBBox        = 1u << 1u,
        WordBasePoint   = 1u << 2u,
        LineBBox        = 1u << 3u,
        PageBBox        = 1u << 4u,
    };
    void set_debug_flag(Debug flag, bool enabled = true);
    void set_debug_flags(DebugFlags flags) { m_debug = flags; }
    bool has_debug_flag(Debug flag) const;

    class Impl;
    Impl& impl() { return *m_impl; }

private:
    std::unique_ptr<Impl> m_impl;
    DebugFlags m_debug = 0;
};


}} // namespace xci::graphics

#endif // XCI_GRAPHICS_VIEW_H
