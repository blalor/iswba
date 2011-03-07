#Set this to @ to keep the makefile quiet
SILENCE = @
ARFLAGS += -c

#---- Outputs ----#
COMPONENT_NAME = iswba

#--- Inputs ----#
CPP_PLATFORM = Gcc
PROJECT_HOME_DIR = ..
CPPUTEST_HOME = $(PROJECT_HOME_DIR)/test/support/CppUTest

CPPUTEST_USE_MEM_LEAK_DETECTION = Y
CPPUTEST_USE_STD_CPP_LIB = N
CPPUTEST_USE_GCOV = Y

CPPUTEST_OBJS_DIR = $(PROJECT_HOME_DIR)/build/objs
CPPUTEST_LIB_DIR  = $(PROJECT_HOME_DIR)/build/lib
CPPUTEST_GCOV_DIR = $(PROJECT_HOME_DIR)/build/gcov

SRC_DIRS = \
	$(PROJECT_HOME_DIR)/main/src

TEST_SRC_DIRS = \
	src 

INCLUDE_DIRS = \
  $(CPPUTEST_HOME)/include \
  $(PROJECT_HOME_DIR)/main/src

# MOCKS_SRC_DIRS = \
# 	mocks 

CPPUTEST_WARNINGFLAGS = -Wall -Wswitch-default -Werror 
CPPUTEST_CFLAGS = -std=c99 
CPPUTEST_CFLAGS += -Wall -Wstrict-prototypes -pedantic
LD_LIBRARIES = -lpthread
  
include $(CPPUTEST_HOME)/build/MakefileWorker.mk
