#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := esp32iot

include $(IDF_PATH)/make/project.mk
include ./main/src/HttpServer/Mongoose/Makefile.projbuild

