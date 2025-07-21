#add include folder
include_directories("${CMAKE_SOURCE_DIR}/include")

add_executable(test_h264_stream test_h264_stream.c)
target_link_libraries(test_h264_stream PRIVATE h264decoder)

add_executable(test_h264_math test_h264_math.c)
target_link_libraries(test_h264_math PRIVATE h264decoder)

add_executable(test_h264_nalu test_h264_nalu.c)
target_link_libraries(test_h264_nalu PRIVATE h264decoder)