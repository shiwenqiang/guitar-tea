all:gt

# Which compiler
CC = gcc

# Options for compiler
CFLAGS = -g -Wall -ansi

# Path
PWD = $(shell pwd)
RESERVED_PATH = /
# Color for echo which's with -e option
# \e[30m -- \e[37m 设置前景色
# \e[30m 将字符的显示颜色改为黑色
# \e[31m 将字符的显示颜色改为红色
# \e[32m 将字符的显示颜色改为绿色
# \e[33m 将字符的显示颜色改为淡红色
# \e[34m 将字符的显示颜色改为蓝色
# \e[35m 将字符的显示颜色改为紫色
# \e[36m 将字符的显示颜色改为淡蓝色
# \e[37m 将字符的显示颜色改为灰色
# 
# \e[40m -- \e[47m 设置背景色
# \e[40m 将背景色设置为黑色
# \e[41m 将背景色设置为红色
# \e[42m 将背景色设置为绿色
# \e[43m 将背景色设置为淡红色
# \e[44m 将背景色设置为蓝色
# \e[45m 将背景色设置为紫色
# \e[46m 将背景色设置为淡蓝色
# \e[47m 将背景色设置为灰色

FGC_BEGIN = \\e[31m
FGC_END = \\e[0m


# Check path
ifneq ($(PWD), $(RESERVED_PATH))

GT_PATH_INCLUDE = $(PWD)/include
GT_PATH_COMMON = $(PWD)/common
GT_PATH_KERNEL = $(PWD)/kernel
GT_PATH_USER = $(PWD)/user

gt:$(GT_PATH_USER)/server.c
	mkdir -p $(PWD)/bin
	$(CC) -o $(PWD)/bin/gt $(CFLAGS) -I$(GT_PATH_INCLUDE) $(GT_PATH_USER)/server.c

.PHONY:clean
clean:
	-rm -f $(PWD)/bin/*
	-rm -rf $(PWD)/bin

else # else for Check path
gt:
	@echo -e $(FGC_BEGIN) "Warning: cannot make file for '$(PWD)'" $(FGC_END)

.PHONY:clean
clean:
	@echo -e $(FGC_BEGIN) "Warning: cannot make clean for '$(PWD)'" $(FGC_END)
endif
