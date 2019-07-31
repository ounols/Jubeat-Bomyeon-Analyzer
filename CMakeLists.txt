cmake_minimum_required(VERSION 3.14)
set(CMAKE_CXX_STANDARD 14)
project(jubeatBMAnalyzer.app)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
include_directories(/usr/include)
include_directories(/usr/local/include)
include_directories(src)

link_directories(/usr/lib)
link_directories(/usr/lib32)
link_directories(/usr/local/lib)





file(GLOB SOURCES_SRC
        "src/*.cpp"
        "src/*.h")


add_executable( jubeatBMAnalyzer.app ${SOURCES_SRC} )

#target_link_libraries( jubeatBMAnalyzer.app opencv_core )
#target_link_libraries( jubeatBMAnalyzer.app opencv_highgui )
#target_link_libraries( jubeatBMAnalyzer.app imgproc )
target_link_libraries( jubeatBMAnalyzer.app ${OpenCV_LIBS} )