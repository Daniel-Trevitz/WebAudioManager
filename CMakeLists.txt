PROJECT(WebAudioManager)
CMAKE_MINIMUM_REQUIRED(VERSION 3.13)
SET(CMAKE_CXX_STANDARD 11)

if(SIMULATION)
    INCLUDE(/home/daniel/Programs/libhttpserver/_install_x86/share/cmake/Modules/FindLibHttpServer.cmake)
    SET(MICROHTTPD /usr/lib/x86_64-linux-gnu/libmicrohttpd.so.12)
    add_definitions(-DSIMULATION)
ELSE()
    set(LIBHTTPSERVER_LIBRARIES httpserver)
    set(LIBHTTPSERVER_INCLUDE_DIRS )

    INCLUDE_DIRECTORIES(/home/daniel/ToolChains/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/sysroot/include)
    LINK_DIRECTORIES(/home/daniel/ToolChains/gcc-linaro-arm-linux-gnueabihf-raspbian-x64/sysroot/lib)

    SET(MICROHTTPD microhttpd)
ENDIF()

SET(CPP_FILES
    main.cpp
    file_response.cpp file_response.h
    directory_server.cpp directory_server.h
    async_process.cpp async_process.h
    channel.cpp channel.h
    status.cpp status.h
    player.cpp player.h
    ripper.cpp ripper.h
    volume.cpp volume.h
    tunein.cpp tunein.h
    radio.cpp radio.h
    bad_request.cpp bad_request.h
    cdinfo.cpp cdinfo.h
)

SET(HTML_FILES
    www/index.html
    www/radio.html
    www/cdrom.html
    www/ripper.html
    www/style.css
    www/interface.js
    www/jquery-3.3.1.min.js
    www/mute.png
    www/not_mute.png
)

ADD_EXECUTABLE(${CMAKE_PROJECT_NAME} ${CPP_FILES})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} PRIVATE ${LIBHTTPSERVER_LIBRARIES} ${MICROHTTPD} curl curlpp)
TARGET_INCLUDE_DIRECTORIES(${CMAKE_PROJECT_NAME} PRIVATE ${LIBHTTPSERVER_INCLUDE_DIRS}/)

ADD_CUSTOM_TARGET(HTML_Files SOURCES ${HTML_FILES})

INSTALL(TARGETS ${CMAKE_PROJECT_NAME} RUNTIME DESTINATION .)
INSTALL(FILES ${HTML_FILES} DESTINATION .)

IF(CMAKE_COMPILER_IS_GNUCC)
   IF(NOT ${CMAKE_OBJCOPY})
      STRING(REGEX REPLACE "gcc$|cc$" "objcopy" CMAKE_OBJCOPY ${CMAKE_C_COMPILER})
   ENDIF()
   MESSAGE("-- Check for objcopy: ${CMAKE_OBJCOPY}")
ELSE()
   MESSAGE("-- Check for objcopy: Not Avaliable!")
ENDIF()

MACRO(STRIP_DEBUG target)
   IF(CMAKE_COMPILER_IS_GNUCC)
      IF(${CMAKE_BUILD_TYPE} MATCHES Debug)
         ADD_CUSTOM_COMMAND(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} --only-keep-debug $<TARGET_FILE:${target}> $<TARGET_FILE:${target}>.debug
            COMMAND ${CMAKE_OBJCOPY} --strip-debug $<TARGET_FILE:${target}>
            COMMAND ${CMAKE_OBJCOPY} --add-gnu-debuglink=$<TARGET_FILE_NAME:${target}>.debug $<TARGET_FILE:${target}>
         )
      ENDIF()
   ELSE()
      MESSAGE("objcopy not avaliable!")
   ENDIF()
ENDMACRO()

STRIP_DEBUG(${CMAKE_PROJECT_NAME})
