### OpenCL-OpenGL Interop Examples

#### Requirements

* CMake (>= 3.16.1)
* OpenCL (>= 1.2)
* GLFW (3.3)

More examples will be added progressively. Current list of examples is given below.

* Julia Sets - Julia fractal images are generated using OpenCL and then rendered onto a OpenGL window
  using texture(the texture is shared between the OCL and OGL contexts).
      Use keys 1-9 to switch between different julia sets.
* Particle Simulation - This is a work in progress, currently it is just random noise changing every
  frame. This example uses intermediate buffer to copy data from OpenCL buffer to OpenGL vertex
buffer object.

#### Note
All examples are by default built for 64-bit machines. If you have need 32-bit executables, please modify the necessary options and rebuild the source files.
