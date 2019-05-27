cmake_minimum_required(VERSION 3.4.1)
add_library( # Sets the name of the library.
            native-lib

            # Sets the library as a shared library.
            SHARED

            # Provides a relative path to your source file(s).
            src/main/cpp/native-lib.cpp )
find_library( # Sets the name of the path variable.
            log-lib

            # Specifies the name of the NDK library that
            # you want CMake to locate.
            log )

# TODO ${CMAKE_SOURCE_DIR}：表示 CMakeLists.txt的当前文件夹路径
# TODO ${ANDROID_ABI}：编译时会自动根据 CPU架构去选择相应的库
set(distribution_DIR ${CMAKE_SOURCE_DIR}/src/main/jniLibs)

add_library(
            avcodec-57
            SHARED
            IMPORTED)
set_target_properties( avcodec-57
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libavcodec-57.so )

add_library( avfilter-6
             SHARED
             IMPORTED)
set_target_properties( avfilter-6
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libavfilter-6.so)

add_library( avformat-57
             SHARED
             IMPORTED)
set_target_properties( avformat-57
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libavformat-57.so)

add_library( avutil-55
             SHARED
             IMPORTED)
set_target_properties( avutil-55
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libavutil-55.so)

add_library( swresample-2
             SHARED
             IMPORTED)
set_target_properties( swresample-2
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libswresample-2.so)

add_library( swscale-4
             SHARED
             IMPORTED)
set_target_properties( swscale-4
                       PROPERTIES IMPORTED_LOCATION
                       ${distribution_DIR}/armeabi/libswscale-4.so)

#add_library( xrapi
#        SHARED
#        IMPORTED)
#set_target_properties( xrapi
#        PROPERTIES IMPORTED_LOCATION
#        ${distribution_DIR}/armeabi/libxrapi.so)

# TODO 引入第三方 .h 文件夹
include_directories(src/main/cpp/include/)

# TODO 将自己编写的 C/C++源文件与第三方库进行连接
target_link_libraries( native-lib
                       avcodec-57
                       avfilter-6
                       avformat-57
                       avutil-55
                       swresample-2
                       swscale-4
#                       xrapi
                       ${log-lib} )

# TODO 引入连接服务器的c++端源文件
 add_library( # 设置库名.
              JNIServerLib
              # 设置库类型为动态库.
              SHARED
              # 列出所有需要编译的c源文件.
              src/main/cpp/com_panoeye_pano_JNIServerLib.cpp
               src/main/cpp/GLManager.cpp
               src/main/cpp/GLMatrix.cpp
               src/main/cpp/MainController.cpp
               src/main/cpp/Online/OnlineDecoder.cpp
               src/main/cpp/Online/OnlinePano.cpp
               src/main/cpp/OnlineSDK/CClient/CClient.cpp
               src/main/cpp/OnlineSDK/CClient/MemoryPool.cpp
               src/main/cpp/OnlineSDK/CClient/NBSocketClient.cpp
               src/main/cpp/OnlineSDK/md5/md5.cpp
               src/main/cpp/OnlineSDK/PEClientSDK/PEClient.cpp
               src/main/cpp/OnlineSDK/PEClientSDK/PEClientSDK.cpp
               src/main/cpp/OnlineSDK/SysComm.cpp
               src/main/cpp/OnlineSDK/XML/tinyxml2.cpp
               src/main/cpp/OnlineSDK/XML/XmlRpcTX2Util.cpp
               src/main/cpp/OnlineSDK/XML/XmlRpcTX2Value.cpp
               src/main/cpp/PER/PERBinFile.cpp
               src/main/cpp/PER/PERDecoder.cpp
               src/main/cpp/PER/PERFile.cpp
               src/main/cpp/PER/PERPackage.cpp
               src/main/cpp/PER/PERPano.cpp
               src/main/cpp/PER/zip/ZipLibUtil.cpp
               src/main/cpp/PES/PESBinFile.cpp
               src/main/cpp/PES/PESDecoder.cpp
               src/main/cpp/PES/PESFile.cpp
               src/main/cpp/PES/PESPano.cpp
               )

  target_link_libraries( # 指定需要链接的目标库.
                        JNIServerLib
                        # 将目标库与log库链接起来
                        ${log-lib}
                        # 源文件中用到的其他库（以下4个必需）.
                        GLESv1_CM
                        GLESv2
                        mediandk
                        z
                        # 源文件中用到的其他库（以下可选）.
                        #c
                        #stdc++
                        #atomic
                        #log
                        #android
                        #EGL
                        #jnigraphics
                        )