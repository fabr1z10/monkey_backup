cmake_minimum_required(VERSION 3.3)

project(monkey)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(yaml-cpp REQUIRED)

add_subdirectory(pybind11)

include_directories(${PYBIND_INCLUDE_DIR})

#add_executable(pino main.cpp shader.cpp)
pybind11_add_module(monkey main.cpp node.cpp component.cpp components/renderer.cpp components/collider.cpp tex.cpp asset_manager.cpp pyfunc.cpp models/sprite.cpp
        models/rawmodel.cpp models/text.cpp util.cpp shapes/convexpoly.cpp font.cpp camera.cpp model.cpp shader.cpp engine.cpp room.cpp
        runners/collisionengine.cpp runners/collisionresponse.cpp bounds.cpp modelmake.cpp components/move.cpp components/controller.cpp
        components/statemachine.cpp keylistener.cpp components/walk2d.cpp components/car2d.cpp shapes/intersector.cpp)

include_directories(${GLEW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${YAML_INCLUDE_DIRS})

target_link_libraries(monkey PUBLIC ${OPENGL_LIBRARIES}  ${PNG_LIBRARY} ${JPEG_LIBRARIES} ${YAML_CPP_LIBRARIES} glfw GLEW::GLEW)

#add_custom_command(
#        TARGET monkey POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy
#        ${CMAKE_SOURCE_DIR}/config.out
#        ${CMAKE_CURRENT_BINARY_DIR}/config.out)

#file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/config.out2 CONTENT "pippo $<TARGET_FILE_NAME:monkey>")
file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/setup.py INPUT ${CMAKE_SOURCE_DIR}/config.out)


message($<TARGET_FILE_NAME:monkey>)