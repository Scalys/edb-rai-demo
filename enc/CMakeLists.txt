# Copyright (c) Open Enclave SDK contributors.
# Licensed under the MIT License.

include(../cmake/add-enclave.cmake)

include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/libs/gemmlowp
    ${CMAKE_SOURCE_DIR}/libs/ruy
    ${CMAKE_SOURCE_DIR}/enc/inc
    ${CMAKE_SOURCE_DIR}/common
)

add_custom_command(
  OUTPUT
    edb_rai_demo_t.h
    edb_rai_demo_t.c
    edb_rai_demo_args.h
  DEPENDS ${CMAKE_SOURCE_DIR}/edb_rai_demo.edl
  COMMAND openenclave::oeedger8r --trusted ${CMAKE_SOURCE_DIR}/edb_rai_demo.edl
    --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include
    --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include/openenclave/edl
    --search-path ${PKG_CONFIG_SYSROOT_DIR}/opt/oe/${OE_OPTEE_PLATFORM}/include/openenclave/edl/optee
)

add_custom_command(
  OUTPUT
    edb_rai_demo_model_data.cc
    edb_rai_demo_model_data.h
  DEPENDS ${CMAKE_SOURCE_DIR}/scripts/generate_cc_arrays.py
  COMMAND
    python3 ${CMAKE_SOURCE_DIR}/scripts/generate_cc_arrays.py ${CMAKE_CURRENT_BINARY_DIR}/edb_rai_demo_model_data.cc ${CMAKE_SOURCE_DIR}/models/edb_rai_demo.tflite &&
    python3 ${CMAKE_SOURCE_DIR}/scripts/generate_cc_arrays.py ${CMAKE_CURRENT_BINARY_DIR}/edb_rai_demo_model_data.h ${CMAKE_SOURCE_DIR}/models/edb_rai_demo.tflite
)

add_enclave(
  TARGET edb_rai_demo_enclave
  UUID c7198089-6313-4ba6-8e3c-47846520df18
    SOURCES
    enc.cc
    classify.cc
    ${CMAKE_CURRENT_BINARY_DIR}/edb_rai_demo_model_data.cc
    ${CMAKE_CURRENT_BINARY_DIR}/edb_rai_demo_t.c
)

target_link_directories(edb_rai_demo_enclave PRIVATE
    ${CMAKE_SOURCE_DIR}/libs/realityai
)
target_link_libraries(edb_rai_demo_enclave RealityAI)

target_link_libraries(edb_rai_demo_enclave
  ${PKG_CONFIG_SYSROOT_DIR}/opt/tflite-micro/tensorflow/lite/micro/tools/make/gen/linux_aarch64_debug/lib/libtensorflow-microlite.a
)

target_link_libraries(edb_rai_demo_enclave openenclave::oeenclave openenclave::oecryptombedtls)
