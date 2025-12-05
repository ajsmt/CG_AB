get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${PROJECT_NAME} CXX)

file(GLOB_RECURSE SOURCES "source/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE "include")


target_link_libraries(${PROJECT_NAME} PRIVATE imgui glfw stb)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts"
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/fonts"
)

file(GLOB IMAGES "${CMAKE_CURRENT_SOURCE_DIR}/*.jpg" "${CMAKE_CURRENT_SOURCE_DIR}/*.png")
if(IMAGES)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        ${IMAGES}
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )
endif()
