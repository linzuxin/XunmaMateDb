CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(root)

SET(CMAKE_CXX_FLAGS "-std=c++11 -O2 -DNDEBUG -DELPP_THREAD_SAFE -DELPP_NO_DEFAULT_LOG_FILE")
MESSAGE("root project name: ${PROJECT_NAME}")
MESSAGE("root source dir: ${PROJECT_SOURCE_DIR}")

ADD_SUBDIRECTORY(src)
ADD_SUBDIRECTORY(tester)

