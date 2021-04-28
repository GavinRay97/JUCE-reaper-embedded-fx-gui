# Reaper TCP/MCP FX UI Embedding with JUCE

```sh-session
$ git clone --recursive --shallow-submodules https://github.com/GavinRay97/JUCE-reaper-embedded-fx-gui
$ cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Debug
$ cmake --build build
```

The CMake build will automatically attempt to apply the JUCE patches to the JUCE submodule from `./patches/JUCE/*.patch` during build.
You can check how this works by looking at `./cmake/patches.cmake`.

It requires a command called `patch` available in your environment.