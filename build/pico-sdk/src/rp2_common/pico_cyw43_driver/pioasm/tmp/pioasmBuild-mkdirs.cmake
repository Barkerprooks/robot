# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/parker/.pico-sdk/tools/pioasm"
  "/home/parker/robot/build/pioasm"
  "/home/parker/robot/build/pioasm-install"
  "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/tmp"
  "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
  "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src"
  "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/parker/robot/build/pico-sdk/src/rp2_common/pico_cyw43_driver/pioasm/src/pioasmBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
