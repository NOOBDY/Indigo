# Indigo

[![build](https://github.com/NOOBDY/Indigo/actions/workflows/build.yml/badge.svg)](https://github.com/NOOBDY/Indigo/actions/workflows/build.yml) 

As of now, this project's structure and code in heavily borrowed from [opengl-tutorial](http://www.opengl-tutorial.org/) and the [Hazel Engine](https://github.com/TheCherno/Hazel) from the Cherno.

## Cloning

To clone the project, run the following command:

```
git clone --recurse-submodules https://github.com/NOOBDY/Indigo.git
```

If the repo was cloned with setting up the submodules, run the following:

```
cd /path/to/Indigo
git submodule init
git submodule update
```

> Some of these submodules are quite large (notedly assimp), but it would require too much work to create those from scratch. It is possible to switch to lighter alternatives but it isn't an urgent issue so they will have to do it for now.

## Building

You will need a system with CMake set up and have suitable compilers installed (GCC and Clang are tested)

To build the project, run the following command. You can add other CMake flags as you see fit

```
cmake -B <builddir> .
cmake --build <builddir>
```

Alternatively, you can use the [CMake Tools Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) from VSCode and have it configure everything for you

## Contributing

Make sure to format the code with `clang-format`. This project currently doesn't follow any major project's styling conventions so some observations might be needed.
