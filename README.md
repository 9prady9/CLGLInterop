OpenCL-OpenGL Interop Examples
==============================

More examples will be added progressively. Current list of examples is given below.

* Fractal - fractal images are generated using OpenCL and then rendered onto a OpenGL window using texture(the texture is shared between the OCL and OGL contexts).

The examples can be built for Windows and Linux. For windows platform, i used Visual Studio 2012 as the development environment. For linux platform, i used standard makefile based build.

Prerequisites:
--------------
* The examples currently run on NVIDIA GPUs only since i am searching only for NVIDIA platform. You can change that to look for AMD GPU or any other acceleration device. To compile the code as is, you would need CUDA toolkit installation.
* GLFW development files are required.
* GLEW developement files are required.



Windows
-------
Open the solution file in Visual Studio 2012. Ensure that the following settings are correct.

1. Include paths for the libraries: `GLEW`, `GLFW` & `CUDA toolkit` are added under `Project Properties->Configuration Properties->VC++ Directories->Include Directories`
2. Library paths for the libraries mentioned in point (1) are added under `Project Properties->Configuration Properties->VC++ Directories->Library Directories`
3. Dependency libs(OpenGL32.lib;OpenCL.lib;glew32.lib;glfw3dll.lib;) are mentioned properly under `Project Properties->Configuration Properties->Linker->Input->Additional Dependencies`.
4. All the dlls for the above mentioned libraries are available to visual studio through `PATH` variable.

If the above stated settings are appropriate, the example should build just fine.

Linux
-----
'Makefile' coming soon. 

There are only two lines in the code that needs to be changed to compile and run the examples on linux. I have already tested the code on linux. I will push those changes as soon as the makefile is ready.


```Note
All examples are by default built for 64-bit machines. If you have need 32-bit executables, please modify the necessary options and rebuild the source files.
```
