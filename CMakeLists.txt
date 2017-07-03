###########################################################################
# Copyright Intel Corp.
# Author: Brendan Le Foll <brendan.le.foll@intel.com>
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

project (af-mraa C)

cmake_minimum_required (VERSION 3.3)

include (GNUInstallDirs)

set (PROJECT_NAME "af-mraa-binding")
set (PROJECT_PRETTY_NAME "")
set (PROJECT_DESCRIPTION "Binding for handling Mraa.io API")
set (PROJECT_VERSION "0.1")

# Appends the cmake/modules path to MAKE_MODULE_PATH variable.
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package (JSON-C REQUIRED)
find_package (PkgConfig)

pkg_check_modules (MRAA REQUIRED mraa>=1.7.0)

add_subdirectory (src)
