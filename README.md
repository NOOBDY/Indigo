# Indigo

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

## Building

You will need a system with CMake set up and have suitable compilers installed (GCC and Clang are tested)

To build the project, run the following command. You can add other CMake flags as you see fit

```
cmake -B <builddir> .
cmake --build <builddir>
```

Alternatively, you can use the [CMake Tools Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) from VSCode and have it configure everything for you

## Contributing

TBA
