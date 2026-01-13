cmake_minimum_required(VERSION 3.13)
project("example_client")

include("${CMAKE_CURRENT_LIST_DIR}/../../cmake/config_client.cmake")

# UC_UNITY_VERSION must be defined before config_client call.
# You can find this version in .exe -> Properties -> Details -> Product version
set(UC_UNITY_VERSION 2021.3.56f2)

config_client(example_client)
