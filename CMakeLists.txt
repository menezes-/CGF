cmake_minimum_required(VERSION 3.5)
project(CGF)

function(cgf_add_example example)
    # adiciona um examplo, criadno um executable, linkando com a biblioteca CGF
    # e coloca o executavel, junto com os arquivos necessários (pasta bin/data), dentro
    # de um diretório na pasta bin/examples
    cmake_parse_arguments(THIS "EXAMPLE" "" "SOURCES" ${ARGN})
    source_group("" FILES ${THIS_SOURCES})
    add_executable(${example} ${THIS_SOURCES})
    target_link_libraries(${example} CGF)
    set_target_properties(${example}
                          PROPERTIES
                          RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin/examples/${example}"
                          )

    # o comando é POST_BUILD de maneira que só é executado após a successful compilation
    # do exemplo
    if (UNIX)
        add_custom_command(TARGET ${example} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E create_symlink
                           ${CMAKE_SOURCE_DIR}/bin/data $<TARGET_FILE_DIR:${example}>/data)
    else ()
        add_custom_command(TARGET ${example} POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy_directory
                           ${CMAKE_SOURCE_DIR}/bin/data $<TARGET_FILE_DIR:${example}>/data)
    endif ()


endfunction()

set(CGF_ROOT ${PROJECT_SOURCE_DIR})


set(CGF_HEADERS
    ${CGF_ROOT}/lib/CGF/include/Collision.h
    ${CGF_ROOT}/lib/CGF/include/Anim.h
    ${CGF_ROOT}/lib/CGF/include/InputManager.h
    ${CGF_ROOT}/lib/CGF/include/Game.h
    ${CGF_ROOT}/lib/CGF/include/VectorUtils.h
    ${CGF_ROOT}/lib/CGF/include/pugixml/pugiconfig.hpp
    ${CGF_ROOT}/lib/CGF/include/pugixml/pugixml.hpp
    ${CGF_ROOT}/lib/CGF/include/TexRect.h
    ${CGF_ROOT}/lib/CGF/include/ClockHUD.h
    ${CGF_ROOT}/lib/CGF/include/FrameClock.h
    ${CGF_ROOT}/lib/CGF/include/EEDebugDraw3.h
    ${CGF_ROOT}/lib/CGF/include/GameState.h
    ${CGF_ROOT}/lib/CGF/include/Physics.h
    ${CGF_ROOT}/lib/CGF/include/Sprite.h
    ${CGF_ROOT}/lib/CGF/include/tmx/QuadTreeNode.h
    ${CGF_ROOT}/lib/CGF/include/tmx/MapLoader.h
    ${CGF_ROOT}/lib/CGF/include/tmx/zlib.h
    ${CGF_ROOT}/lib/CGF/include/tmx/Helpers.h
    ${CGF_ROOT}/lib/CGF/include/tmx/MapObject.h
    ${CGF_ROOT}/lib/CGF/include/tmx/zconf.h
    ${CGF_ROOT}/lib/CGF/include/TextureManager.h
    )

set(CGF_SOURCES
    ${CGF_ROOT}/lib/CGF/source/TextureManager.cpp
    ${CGF_ROOT}/lib/CGF/source/EEDebugDraw3.cpp
    ${CGF_ROOT}/lib/CGF/source/Physics.cpp
    ${CGF_ROOT}/lib/CGF/source/tmxloader/MapLoaderPublic.cpp
    ${CGF_ROOT}/lib/CGF/source/tmxloader/QuadTreeNode.cpp
    ${CGF_ROOT}/lib/CGF/source/tmxloader/MapLoaderPrivate.cpp
    ${CGF_ROOT}/lib/CGF/source/pugixml/pugixml.cpp
    ${CGF_ROOT}/lib/CGF/source/Sprite.cpp
    ${CGF_ROOT}/lib/CGF/source/Game.cpp
    ${CGF_ROOT}/lib/CGF/source/Collision.cpp
    ${CGF_ROOT}/lib/CGF/source/InputManager.cpp
    )


#=====================SFML=====================
set(SFML_ROOT lib/SFML-2.4.0)
add_subdirectory(lib/SFML-2.4.0)
#=====================SFML=====================

# a partir daqui é necessário desabilitar as "shared libraries" se não a Box2D começa a dar problema
# (https://github.com/erincatto/Box2D/wiki/FAQ#can-i-use-box2d-in-a-dll).
# existem soluções melhores para isso:
# 1) desacoplara biblioteca Box2D da bilioteca
# 2) tentar compilar a Box2D com -fPIC e --whole-archive (não é garantido que vá funcionar)

#=====================BOX2D=====================
set(BOX2D_ROOT ${CGF_ROOT}/lib/Box2D-2.3.0)
if (BUILD_SHARED_LIBS)
    set(BOX2D_BUILD_SHARED TRUE)
    set(BOX2D_LIB Box2D_shared)
else ()
    set(BOX2D_BUILD_STATIC TRUE)
    set(BOX2D_LIB Box2D)

endif ()
add_subdirectory(${BOX2D_ROOT}/include/Box2D)

#=====================BOX2D=====================

#=====================CGF=====================
add_library(CGF ${CGF_HEADERS} ${CGF_SOURCES})

# LTO
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-flto GXX_HAS_LTO_FLAG)
message(STATUS "GXX_HAS_LTO_FLAG = ${GXX_HAS_LTO_FLAG}")

target_compile_options(CGF PUBLIC
                       -std=c++11 -Wno-ignored-qualifiers
                       $<$<CONFIG:Debug>:-g -O0>
                       $<$<CONFIG:Release>:-g0 -O2 -s $<$<BOOL:${GXX_HAS_LTO_FLAG}>:-flto>>)


# se for linux usa o find_packages, se for windows
# linka direto com o zlib1.dll
if (WIN32)
    target_link_libraries(CGF SHARED bin/zlib1.dll)
else ()
    find_package(ZLIB REQUIRED)
    target_include_directories(CGF PUBLIC ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(CGF ${ZLIB_LIBRARIES})
endif ()


target_include_directories(CGF PUBLIC ${CGF_ROOT}/lib/CGF/include)
target_include_directories(CGF PUBLIC ${SFML_ROOT}/include)
target_include_directories(CGF PUBLIC ${BOX2D_ROOT}/include)
target_include_directories(CGF PUBLIC ${CGF_ROOT}/lib/glew-1.10/include)
target_link_libraries(CGF ${BOX2D_LIB} sfml-system sfml-window sfml-graphics sfml-audio ${ZLIB_LIBRARIES})

#=====================CGF=====================

######################EXEMPLOS######################

set(EXAMPLE_OUTPUT ${PROJECT_SOURCE_DIR}/bin/example CACHE PATH "Aonde vão os binários dos exemplos")
set(BUILD_EXAMPLES TRUE CACHE BOOL "Compilar os exemplos")

if (BUILD_EXAMPLES)
    set(E1
        examples/01_Movement/main.cpp
        examples/01_Movement/MenuState.cpp
        examples/01_Movement/MenuState.h
        examples/01_Movement/PlayState.cpp
        examples/01_Movement/PlayState.h
        )

    set(E2
        examples/02_Sprites/main.cpp
        examples/02_Sprites/PlayState.cpp
        examples/02_Sprites/PlayState.h
        )

    set(E3
        examples/03_Tiling/main.cpp
        examples/03_Tiling/PlayState_Solution.cpp
        examples/03_Tiling/PlayState_Solution.h
        )

    set(E4
        examples/04_Collisions/main.cpp
        examples/04_Collisions/PlayState_Solution.cpp
        examples/04_Collisions/PlayState_Solution.h
        )

    set(E5
        examples/05_Physics/main.cpp
        examples/05_Physics/PlayState_Solution.cpp
        examples/05_Physics/PlayState_Solution.h
        )

    set(E6
        examples/06_Audio/main.cpp
        examples/06_Audio/PlayState_Solution.cpp
        examples/06_Audio/PlayState_Solution.h
        )

    set(E7
        examples/07_IA/main.cpp
        examples/07_IA/PlayState.cpp
        examples/07_IA/PlayState.h
        )


    cgf_add_example(01_Movement EXAMPLE SOURCES ${E1})
    cgf_add_example(02_Sprites EXAMPLE SOURCES ${E2})
    cgf_add_example(03_Tiling EXAMPLE SOURCES ${E3})
    cgf_add_example(04_Collisions EXAMPLE SOURCES ${E4})
    cgf_add_example(05_Physics EXAMPLE SOURCES ${E5})
    cgf_add_example(06_Audio EXAMPLE SOURCES ${E6})
    cgf_add_example(07_IA EXAMPLE SOURCES ${E7})
endif ()



