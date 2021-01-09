# 指定最低版本
cmake_minimum_required(VERSION 3.4.1)

# Specifies a library name, specifies whether the library is STATIC or
# SHARED, and provides relative paths to the source code. You can
# define multiple libraries by adding multiple add_library() commands,
# and CMake builds them for you. When you build your app, Gradle
# automatically packages shared libraries with your APK.

#生成库文件，指定库的名字，指定是静态库还是共享库，提供相关的源码路径，可多次使用
add_library( # Specifies the name of the library.
        native-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        c/native_lib.cpp
        c/jni_common/jni_wrapper.cpp
        )
# 生成可执行文件
# add_executable()


# 指定头文件链接路径
include_directories(c/jni_common)


#find_library 命令以找到 NDK 库并将其路径存储为一个变量。您可以使用此变量在构建脚本的其他部分引用 NDK 库。
find_library( # Defines the name of the path variable that stores the
        # location of the NDK library.
        log-lib

        # Specifies the name of the NDK library that
        # CMake needs to locate.
        log)


# 关联指定库文件
## Links your native library against one or more other native libraries.
target_link_libraries( # Specifies the target library.
        native-lib

        # Links the log library to the target library.
        ${log-lib})
#

# 引入第三方库，IMPORTED 标志指示 CMake 想要将此库导入到项目中
#add_library(curl STATIC IMPORTED)
#set_target_properties(curl PROPERTIES IMPORTED_LOCATION
#        ${CMAKE_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI}/libcurl.a)

# 设置变量
#set( lib_src_DIR ../gmath )
#
## Sets lib_build_DIR to the path of the desired output directory.
#set( lib_build_DIR ../gmath/outputs )

# 创建文件
#file(MAKE_DIRECTORY ${lib_build_DIR})


# 包含其他 CMake 项目
#add_subdirectory( # Specifies the directory of the CMakeLists.txt file.
#                  ${lib_src_DIR}
#
#                  # Specifies the directory for the build outputs.
#                  ${lib_build_DIR} )