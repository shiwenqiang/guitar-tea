###############################################################
#          Copyright 2019. Guitar&Tea Tech. Co., Ltd.
#                     All Rights Reseved
#
# File     :CMakeLists.txt
# Module   :CMake
# Date     :2019/05/04
# Author   :shiwenqiang
# Brief    :
#
#------------------------Revision History----------------------
#  No. Version Date        Mender        Desc.
#  1   V0.01   2019-05-04  shiwenqiang   Original Version.
###############################################################

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include
                    ${PROJECT_SOURCE_DIR}/user/comm
                    ${PROJECT_SOURCE_DIR}/user/core)

FILE(GLOB SPINLOCK_TEST ${PROJECT_SOURCE_DIR}/test/spinlock_test.c)
FILE(GLOB CORE ${PROJECT_SOURCE_DIR}/user/core/*.c)

SET(CMAKE_C_FLAGS "-Wall -g -ggdb -pthread -rdynamic -ldl")
ADD_DEFINITIONS(-D__STATIC_INLINE__)

FOREACH (VAR_FILE_PATH ${SPINLOCK_TEST} ${CORE})
    MESSAGE(STATUS "${VAR_FILE_PATH}")
ENDFOREACH (VAR_FILE_PATH)

ADD_EXECUTABLE(spinlock_test ${SPINLOCK_TEST} ${CORE})