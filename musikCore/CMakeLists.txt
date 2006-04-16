##################################
#
# set project name
# 
##################################

PROJECT(musikCore)

##################################
#
# get misc build flags
#
##################################

FIND_PROGRAM(CMAKE_TAGLIB_CONFIG taglib-config /usr/bin /usr/local/bin)

IF(CMAKE_TAGLIB_CONFIG)
    EXEC_PROGRAM(${CMAKE_TAGLIB_CONFIG}
        ARGS "--cflags"
        OUTPUT_VARIABLE CMAKE_TAGLIB_CFLAGS)
ENDIF(CMAKE_TAGLIB_CONFIG)

##################################
#
# find target link libraries
#
##################################

FIND_LIBRARY(SQLITE_LIBRARY NAMES sqlite3 PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(XINE_LIBRARY NAMES xine PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(OPENTHREADS_LIBRARY NAMES OpenThreads PATHS /usr/lib /usr/local/lib)
FIND_LIBRARY(TAG_LIBRARY NAMES tag PATHS /usr/lib /usr/local/lib)

##################################
#
# set musikCore source files
#
##################################

ADD_LIBRARY( musikCore
	${musikCore_SOURCE_DIR}/src/Equ
	${musikCore_SOURCE_DIR}/src/Fftsg_fl
	${musikCore_SOURCE_DIR}/src/musikAlarm
	${musikCore_SOURCE_DIR}/src/musikBatchAdd
	${musikCore_SOURCE_DIR}/src/musikBatchRetag
	${musikCore_SOURCE_DIR}/src/musikCrossfader
	${musikCore_SOURCE_DIR}/src/musikDir
	${musikCore_SOURCE_DIR}/src/musikEQSettings
	${musikCore_SOURCE_DIR}/src/musikFunctor
	${musikCore_SOURCE_DIR}/src/musikPlayer
	${musikCore_SOURCE_DIR}/src/musikPlayerXINE
	${musikCore_SOURCE_DIR}/src/musikPlayerFMOD
	${musikCore_SOURCE_DIR}/src/musikPlaylist
	${musikCore_SOURCE_DIR}/src/musikRemoveOld
	${musikCore_SOURCE_DIR}/src/musikTagToFn
	${musikCore_SOURCE_DIR}/src/musikTagFromFn
	${musikCore_SOURCE_DIR}/src/musikTask
	${musikCore_SOURCE_DIR}/src/musikTaskQueue
	${musikCore_SOURCE_DIR}/src/musikBatchChgCase
	${musikCore_SOURCE_DIR}/src/musikReloadTags
	${musikCore_SOURCE_DIR}/src/musikLibrary
	${musikCore_SOURCE_DIR}/src/musikLibraryPatches
	${musikCore_SOURCE_DIR}/src/musikFilename
	${musikCore_SOURCE_DIR}/src/unicode
	)

##################################
#
# link against needed libraries
#
##################################

TARGET_LINK_LIBRARIES(musikCore
	${SQLITE_LIBRARY}
	${XINE_LIBRARY}
	${OPENTHREADS_LIBRARY}
	${TAG_LIBRARY}
	)

##################################
#
# set build variables
#
##################################

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_TAGLIB_CFLAGS} -I./src/ -Wall -ansi -pedantic")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -I./src/ -O3 -pipe -fomit-frame-pointer")

##################################
#
# install
#
##################################
SET(CMAKE_INSTALL_PREFIX /usr)
INSTALL_FILES(/include/musikCore .h include/StdString)
INSTALL_FILES(/include/musikCore .h include/musikAlarm)
INSTALL_FILES(/include/musikCore .h include/musikBatchAdd)
INSTALL_FILES(/include/musikCore .h include/musikBatchRetag)
INSTALL_FILES(/include/musikCore .h include/musikBatchChgCase)
INSTALL_FILES(/include/musikCore .h include/musikConfig)
INSTALL_FILES(/include/musikCore .h include/musikCrossfader)
INSTALL_FILES(/include/musikCore .h include/musikDir)
INSTALL_FILES(/include/musikCore .h include/musikEQSettings)
INSTALL_FILES(/include/musikCore .h include/musikFilename)
INSTALL_FILES(/include/musikCore .h include/musikFunctor)
INSTALL_FILES(/include/musikCore .h include/musikLibrary)
INSTALL_FILES(/include/musikCore .h include/musikPlayer)
INSTALL_FILES(/include/musikCore .h include/musikPlayerFMOD)
INSTALL_FILES(/include/musikCore .h include/musikPlayerXINE)
INSTALL_FILES(/include/musikCore .h include/musikPlayerBASS)
INSTALL_FILES(/include/musikCore .h include/musikPlaylist)
INSTALL_FILES(/include/musikCore .h include/musikRemoveOld)
INSTALL_FILES(/include/musikCore .h include/musikReloadTags)
INSTALL_FILES(/include/musikCore .h include/musikTagFromFn)
INSTALL_FILES(/include/musikCore .h include/musikTagToFn)
INSTALL_FILES(/include/musikCore .h include/musikTask)
INSTALL_FILES(/include/musikCore .h include/musikTaskQueue)
INSTALL_FILES(/include/musikCore .h include/musikCore)
INSTALL_FILES(/include/musikCore .hpp include/paramlist)
INSTALL_FILES(/include/musikCore .h src/unicode)
INSTALL_FILES(/lib .a libmusikCore)

