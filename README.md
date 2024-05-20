# Indigo

[![build](https://github.com/NOOBDY/Indigo/actions/workflows/build.yml/badge.svg)](https://github.com/NOOBDY/Indigo/actions/workflows/build.yml) 

![](https://cdn.discordapp.com/attachments/973063566993268826/1214435063035469885/image.png?ex=65f919ca&is=65e6a4ca&hm=8ffd7fd5a650e1d856e7d70897e127b80f891e593efa23472c52c2e9c0046a51&)

As of now, this project's structure and code in heavily referenced from [opengl-tutorial](http://www.opengl-tutorial.org/), [Learn OpenGL](https://learnopengl.com/) and the [Hazel Engine](https://github.com/TheCherno/Hazel) from the Cherno.

## Cloning

To clone the project, run the following command:

```
git clone --recurse-submodules https://github.com/NOOBDY/Indigo.git
```

If the repo was cloned without setting up the submodules, run the following:

```
cd /path/to/Indigo
git submodule update --init
```

> Some of these submodules are quite large (notedly assimp), but it would require too much work to create those from scratch. It is possible to switch to lighter alternatives but it isn't an urgent issue so they will have to do it for now.

To fetch the assets, [`git-lfs`](https://git-lfs.github.com/) is required

```
git lfs fetch
git lfs checkout
```

## Building

You will need a system with CMake set up and have suitable compilers installed (mainly tested on Linux x86_64/Clang 14)

To build the project, run the following command. You can add other CMake flags as you see fit

```
cmake -B <builddir> .
cmake --build <builddir>
```

Alternatively, you can use the [CMake Tools Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) from VSCode and have it configure everything for you

## Contributing

Make sure to format the code with `clang-format` and run static checking through `clang-tidy`. This project currently doesn't follow any major project's styling conventions so some observations might be needed.
