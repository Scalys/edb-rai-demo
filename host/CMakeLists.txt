add_custom_command(
  OUTPUT
    edb_rai_demo_u.h
    edb_rai_demo_u.c
    edb_rai_demo_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/edb_rai_demo.edl
  COMMAND openenclave::oeedger8r --untrusted ${CMAKE_SOURCE_DIR}/edb_rai_demo.edl
  --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include
  --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include/openenclave/edl
  --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include/openenclave/edl/optee)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/common
)

add_executable(edb_rai_demo
    host.cc
    main.cc
    audio.cc
    ${CMAKE_CURRENT_BINARY_DIR}/edb_rai_demo_u.c)

target_include_directories(edb_rai_demo PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(edb_rai_demo openenclave::oehost)

target_link_directories(edb_rai_demo PRIVATE
  /home/haff/work/oekey/openenclave-sdk-builder/packages/edb-rai-demo/libs/realityai
#    ${CMAKE_SOURCE_DIR}/libs/realityai
)
target_link_libraries(edb_rai_demo RealityAI)
