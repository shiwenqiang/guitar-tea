#**************************************************************
#*          Copyright 2015. Guitar&Tea Tech. Co., Ltd.
#*                     All Rights Reseved
#*
#* File     :Makefile
#* Module   :make Compile config file
#* Date     :2017/10/11
#* Author   :shiwenqiang
#* Brief    :Makefile.
#*
#*------------------------Revision History----------------------
#*  No. Version Date        Mender        Desc.
#*  1   V0.01   2017-10-11  shiwenqiang   Original Version.
#**************************************************************

#------------------------------------------------
#	Define compiler
CC = gcc

CFLAGS = -g -Wall -pthread

CURRENT_PATH = $(shell pwd)
RESERVED_PATH = /

#	Get src *.c list
sources := $(wildcard $(CURRENT_PATH)/../user/core/*.c)

#	Get obj *.o list
objects := $(patsubst %.c,%.o,$(sources))

#	Get dep *.d list
dependence := $(objects:.o=.d)

#	Define Macros

GT_PATH_INCLUDE = $(CURRENT_PATH)/../include
GT_PATH_COMMON = $(CURRENT_PATH)/../common
GT_PATH_KERNEL = $(CURRENT_PATH)/../kernel
GT_PATH_USER = $(CURRENT_PATH)/../user/core
#------------------------------------------------
#	Prepare Dir
$(shell mkdir -p $(CURRENT_PATH)/bin)
$(shell mkdir -p $(CURRENT_PATH)/obj)
$(shell mkdir -p $(CURRENT_PATH)/dep)

#------------------------------------------------
#	Define Target & Dependency-relationship
all: gt
.PHONY: all

gt: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(CURRENT_PATH)/bin/gt
%.o: %.c
	$(CC) $(CFLAGS) -I$(GT_PATH_INCLUDE) -c $< -o $@

#------------------------------------------------
#	Include dependence file *.d
include $(dependence)

#	Define func of generate dependence-relationship file *.d
define gen_dep
@set -e; rm -f $@;\
$(CC) -MM $(CFLAGS) -I$(GT_PATH_INCLUDE) $< > $@.$$$$;\
sed 's,\($(subst $(GT_PATH_USER)/,,$*)\)\.o[ :]*,$*.o $@: ,g' < $@.$$$$ > $@;\
rm -f $@.$$$$
endef

%.d: %.c
	$(gen_dep)

.PHONY: clean
clean:
	-rm -f all $(objects) $(dependence)
	-rm -f $(GT_PATH_USER)/*.d $(GT_PATH_USER)/*.o
echo:
	@echo sources=$(sources)
	@echo objects=$(objects)
	@echo dependence=$(dependence)
	@echo CFLAGS=$(CFLAGS)
