#############################################################################
#
# Makefile for librf24mesh examples on Raspberry Pi
#
# By:  TMRh20
# Date:    2013/09
#
# Description:
# ------------
# use make all and make install to install the examples
# You can change the install directory by editing the prefix line
#
prefix := /usr/local

# Which compiler to use
CC=g++

ARCH=armv6zk
ifeq "$(shell uname -m)" "armv7l"
ARCH=armv7-a
endif

# Detect the Raspberry Pi from cpuinfo
#Count the matches for BCM2708 or BCM2709 in cpuinfo
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2708)
ifneq "${RPI}" "1"
RPI=$(shell cat /proc/cpuinfo | grep Hardware | grep -c BCM2709)
endif

ifeq "$(RPI)" "1"
# The recommended compiler flags for the Raspberry Pi
CCFLAGS=-Ofast -mfpu=vfp -mfloat-abi=hard -march=$(ARCH) -mtune=arm1176jzf-s -std=c++0x
endif

# define all programs
PROGRAMS = rpi_nrf24_master
# SOURCES = ${PROGRAMS:=.cpp}
# SOURCES=*.cpp
OBJDIR=obj
OBJSRESTWEB=$(addprefix $(OBJDIR)/, ${PROGRAMS:=.o} \
        configUtils.o \
		nrf24Utils.o \
		mqttUtils.o \
		restweb.o \
		restwebHandler.o \
		TEA.o \
		parser.o)
OBJS=$(addprefix $(OBJDIR)/, ${PROGRAMS:=.o} \
        configUtils.o \
		nrf24Utils.o \
		mqttUtils.o \
		TEA.o \
		parser.o)

CPPLIB=-lpthread -lboost_system -lcrypto -lssl
CPPRESTLIB=-lcpprest
CPPNRFLIB=-lrf24-bcm -lrf24network -lrf24mesh
CPPMQTTLIB=-lmosquitto
BINDIR=bin

all: ${PROGRAMS}

${PROGRAMS}: ${OBJS}
	mkdir -p ${BINDIR}
	# $(CC) ${CCFLAGS} -Wall -I../ -lrf24-bcm -lrf24network -lrf24mesh ${CPPRESTLIB} $@.cpp -o bin/$@
	$(CC) ${CCFLAGS} -Wall -I../ ${CPPNRFLIB} ${CPPLIB} ${CPPMQTTLIB} $^ -o ${BINDIR}/$@
	cp config/*.conf ${BINDIR}/

${OBJDIR}/%.o: src/%.cpp
	mkdir -p ${OBJDIR}
	$(CC) ${CCFLAGS} -Wall -I../ -c $< -o $@

clean:
	rm -rf ${OBJDIR}/*.o ${BINDIR}/${PROGRAMS}
	
install: all
	test -d $(prefix) || mkdir $(prefix)
	test -d $(prefix)/${BINDIR} || mkdir $(prefix)/${BINDIR}
	for prog in $(PROGRAMS); do \
	  install -m 0755 $$prog $(prefix)/${BINDIR}; \
	done

.PHONY: install
