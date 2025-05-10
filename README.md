[Flatova Icon](fl_logo_iter1.png)

# FLATOVA
An **OpenGL based rendering engine**, featuring simplistic **Entity Component System(ECS)**, and written conforming **C89 standard**.

*INSERT PICTURES HERE*

---
## Installation

#### Dependencies
1. Any C89 standard capable compiler
2. [Meson Build System](https://mesonbuild.com/)

#### Windows
> NOTE: using of MSYS2 is not recommended as it has certain header files that collides with the project's variable names! 
// TODO

#### Linux
// TODO

#### Unix
// TODO

#### MacOS
// TODO


## Compilation Instructions
open cloned repository in terminal, then type the following command:
```
meson setup build
```
this allows Meson to setup the build directory for your own operating system. It will hopefully find most of the required dependencies and compilers from your system.

```
meson compile -C build
```
This will prompt Meson to compile the project utilizing the build directory you have just generated.

this should utilize the [Meson Build System](https://mesonbuild.com/) to compile the project.


## Contribution Guide
See:
1. Project Structure (Incomplete)
2. Issue tracking (Incomplete)


---
Licensed under **MIT** by CXRedix.
