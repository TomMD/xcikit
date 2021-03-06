Coordinates
===========

First, let's look how Vulkan transforms coordinates when rendering
primitives.

Then, we'll create a coordinate system focused on rendering scalable UI.


Vulkan Coordinates
------------------

### Viewport coordinates

Vulkan uses following viewport coordinates:
- X axis goes from left to right
- Y axis goes from top to bottom
- Z axis goes from near to far

Note that Y axis is inverted in OpenGL, but that was fixed in Vulkan.
It now goes the same direction as is usual in window coordinates.

The coordinates are normalized to interval -1.0 .. 1.0:
- Top left corner is X= -1.0, Y= -1.0
- Bottom right corner is X= 1.0, Y= 1.0
- Most distant point has Z= 1.0
- The nearest point has Z= 0.0

Anything outside these limits is clipped off.

The normalized coordinates are mapped to actual viewport, so (-1.0, -1.0)
becomes (0, 0) in pixels and (1.0, 1.0) becomes something like (799, 599).

### Projection Matrix

Now let's add a matrix for projection transformation.

In vertex shader, we'll multiply our vertex coordinates by projection matrix:

    in vec2 position;
    uniform mat4 projection;

    void main() {
        gl_Position = projection * vec4(position, 0.0, 1.0);
    }

The matrix will be supplied as an uniform:

    const GLfloat projection[] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
    };

This is identity matrix and as such, it does nothing to our vertices.

Note, that Vulkan matrices are column-major, so they look transposed when
compared to standard mathematical notation:

    GLfloat gl_matrix[] = {  //  In math:
        ax, ay, az, aw,      //  | ax  bx  cx  dx |
        bx, by, cz, bw,      //  | ay  by  cy  dy |
        cx, cy, cz, cw,      //  | az  bz  cz  dz |
        dx, dy, dz, dw,      //  | aw  bw  cw  dw |
    };                       //

To adjust the 2D view without touching Z coordinate, we can adjust
some of the fields:

- Change `ax`, `by` to scale the view, eg. set both to 0.5 to resize
  the view to (-2.0, 2.0) in both axes. Or, if we'd consider the model
  being rendered, this will scale it to half size.

- Change `dx`, `dy` to translate the view (or model).

### Aspect Ratio

Let's add aspect ratio to our projection matrix:

    GLfloat window_width;
    GLfloat window_height;

    GLfloat aspect_ratio = window_width / window_height;
    const GLfloat projection[] = {
            1.0f / aspect_ratio, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f,
    };

This expands the view in horizontal direction, so if we draw a quad
with coordinates from (-0.5,-0.5) to (0.5, 0.5) it will always look like
a square, regardless of actual viewport size.


XCI Coordinates
---------------

For the purpose of rendering UI and text, we'll stay with the Vulkan
coordinates, with one modification:

- One of the X/Y axis is expanded, so the visible coordinates go beyond -1/1.
  This is the aspect ratio correction.

We no longer need to flip Y coordinate as we did in OpenGL.
Vulkan Y coordinate is well suited for drawing text or widgets, because
we usually write from top to bottom.
 
The diagram shows that the square of (-1,-1) .. (1,1) is always completely
covered by the view. Depending on actual ratio of width and height,
it is expanded either horizontally or vertically.

    horizontal+           vertical+
    +---+-------+---+     +---------+
    |   |       |   |     |    +    |
    | + |  2x2  | + |     +---------+
    |   |       |   |     |         |
    +---+-------+---+     |   2x2   |
                          |         |
                          +---------+
                          |    +    |
                          +---------+

Same as with Vulkan coordinates, origin (0,0) is in the center of the screen.

These coordinates are used for positioning text and widgets in the viewport.
There is always the same amount of elements, disregarding the actual size
of window in pixels. In bigger window or on finer screen, the elements just
get bigger and more detailed.

These are called *scalable units* throughout xcikit.

There are also two other kinds of units:
- *Framebuffer pixels* - these are based on size of frame buffer
  (the actual pixels being drawn on the screen)
- *Screen pixels* - these are based on size of window as reported by OS
  (the pixels with "normal" size)

The screen pixels might be the same as framebuffer pixels, or they might
be bigger (this technique is used for high-DPI screens).

See `demo_coords` in examples for live demonstration of all kinds of the
coordinates.
