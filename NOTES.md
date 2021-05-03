# Notes

## Configuring CMake ExternalProject to build with patches properly

https://gitlab.kitware.com/cmake/cmake/-/issues/17287#note_841386

```cmake
function(my_ExternalProject_add_patches name)
    ExternalProject_Get_Property(${name} STAMP_DIR)
    ExternalProject_Get_Property(${name} SOURCE_DIR)

    foreach(patch ${ARGN})
        list(APPEND absolute_paths "${CMAKE_CURRENT_SOURCE_DIR}/${patch}")
    endforeach()

    set(patch_command patch -d "${SOURCE_DIR}" -p1 -i)
    foreach(patch ${absolute_paths})
        list(APPEND cmd_list COMMAND ${patch_command} "${patch}")
    endforeach()

    add_custom_command(
        APPEND
        OUTPUT ${STAMP_DIR}/${name}-download
        ${cmd_list}
        DEPENDS ${absolute_paths}
    )
endfunction()
```
