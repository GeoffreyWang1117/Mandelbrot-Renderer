cmake_minimum_required(VERSION 3.10)
project(FractalsWASM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT EMSCRIPTEN)
    message(FATAL_ERROR "This project must be built with Emscripten. Use: emcmake cmake ..")
endif()

add_executable(fractals src/fractals_wasm.cpp)

set_target_properties(fractals PROPERTIES
    SUFFIX ".js"
    LINK_FLAGS "-s WASM=1 -s EXPORTED_RUNTIME_METHODS=['cwrap','ccall','HEAPU8'] -s MODULARIZE=1 -s EXPORT_NAME='FractalsModule' -s ALLOW_MEMORY_GROWTH=1 -s MAXIMUM_MEMORY=2GB -s EXPORTED_FUNCTIONS=['_malloc','_free','_testFunction','_mandelbrotIterations','_juliaIterations','_burningShipIterations','_newtonIterations','_tricornIterations','_phoenixIterations','_computeFractalBatch','_renderFractalImage'] -O3 -s ASSERTIONS=1 --bind"
)

target_compile_options(fractals PRIVATE -O3 -ffast-math -DNDEBUG)

# Copy build output to docs/ for GitHub Pages deployment
add_custom_command(TARGET fractals POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy fractals.js ${CMAKE_SOURCE_DIR}/../docs/
    COMMAND ${CMAKE_COMMAND} -E copy fractals.wasm ${CMAKE_SOURCE_DIR}/../docs/
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Copying WASM files to docs/"
)
