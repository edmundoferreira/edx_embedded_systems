#******************************************************************************
#
# Makefile - Rules for building the blinky example.
#
# Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
#
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
#
# This is part of revision 2.0.1.11577 of the EK-TM4C123GXL Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=TM4C123GH6PM

#
# The base directory for TivaWare.
#

#
# Include the common make definitions.
#
#include makedefs



#******************************************************************************
#
# Get the operating system name.  If this is Cygwin, the .d files will be
# munged to convert c: into /cygdrive/c so that "make" will be happy with the
# auto-generated dependencies.
#
#******************************************************************************
os:=${shell uname -s}

#******************************************************************************
#
# The compiler to be used.
#
#******************************************************************************
ifndef COMPILER
COMPILER=gcc
endif

#******************************************************************************
#
# Definitions for using GCC.
#
#******************************************************************************
ifeq (${COMPILER}, gcc)

#
# Get the prefix for the tools to use.  Use arm-stellaris-eabi if it exists,
# otherwise fall back to arm-none-eabi.
#
PREFIX:=${shell type arm-stellaris-eabi-gcc > /dev/null 2>&1 && \
         echo arm-stellaris-eabi || echo arm-none-eabi}

#
# The command for calling the compiler.
#
CC=${PREFIX}-gcc

#
# The location of the C compiler
# ARMGCC_ROOT is used by some makefiles that need to know where the compiler
# is installed.
#
ARMGCC_ROOT:=${shell dirname '${shell sh -c "which ${CC}"}'}/..

#
# Set the compiler CPU/FPU options.
#
CPU=-mcpu=cortex-m4
FPU=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp

#
# The flags passed to the assembler.
#
AFLAGS=-mthumb \
       ${CPU}  \
       ${FPU}  \
       -MD

#
# The flags passed to the compiler.
#
CFLAGS=-mthumb             \
       ${CPU}              \
       ${FPU}              \
       -ffunction-sections \
       -fdata-sections     \
       -MD                 \
       -std=c99            \
       -Wall               \
       -pedantic           \
       -DPART_${PART}      \
       -c

#
# The command for calling the library archiver.
#
AR=${PREFIX}-ar

#
# The command for calling the linker.
#
LD=${PREFIX}-ld

#
# The flags passed to the linker.
#
LDFLAGS=--gc-sections

#
# Get the location of libgcc.a from the GCC front-end.
#
LIBGCC:=${shell ${CC} ${CFLAGS} -print-libgcc-file-name}

#
# Get the location of libc.a from the GCC front-end.
#
LIBC:=${shell ${CC} ${CFLAGS} -print-file-name=libc.a}

#
# Get the location of libm.a from the GCC front-end.
#
LIBM:=${shell ${CC} ${CFLAGS} -print-file-name=libm.a}

#
# The command for extracting images from the linked executables.
#
OBJCOPY=${PREFIX}-objcopy

#
# Tell the compiler to include debugging information if the DEBUG environment
# variable is set.
#
ifdef DEBUG
CFLAGS+=-g -D DEBUG -O0
else
CFLAGS+=-Os
endif


#
# Add the tool specific CFLAGS.
#
CFLAGS+=${CFLAGSgcc}

#
# Add the include file paths to AFLAGS and CFLAGS.
#
AFLAGS+=${patsubst %,-I%,${subst :, ,${IPATH}}}
CFLAGS+=${patsubst %,-I%,${subst :, ,${IPATH}}}

#
# The rule for building the object file from each C source file.
#
${COMPILER}${SUFFIX}/%.o: %.c
	@if [ 'x${VERBOSE}' = x ];                            \
	 then                                                 \
	     echo "  CC    ${<}";                             \
	 else                                                 \
	     echo ${CC} ${CFLAGS} -D${COMPILER} -o ${@} ${<}; \
	 fi
	@${CC} ${CFLAGS} -D${COMPILER} -o ${@} ${<}
ifneq ($(findstring CYGWIN, ${os}), )
	@if [ -e ${@:.o=.d} ];                                   \
	then                                                     \
		sed -i -r 's/ ([A-Za-z]):/ \/cygdrive\/\1/g' ${@:.o=.d} ; \
	fi
endif

#
# The rule for building the object file from each assembly source file.
#
${COMPILER}${SUFFIX}/%.o: %.S
	@if [ 'x${VERBOSE}' = x ];                               \
	 then                                                    \
	     echo "  AS    ${<}";                                \
	 else                                                    \
	     echo ${CC} ${AFLAGS} -D${COMPILER} -o ${@} -c ${<}; \
	 fi
	@${CC} ${AFLAGS} -D${COMPILER} -o ${@} -c ${<}
ifneq ($(findstring CYGWIN, ${os}), )
	@if [ -e ${@:.o=.d} ];                                   \
	then                                                     \
		sed -i -r 's/ ([A-Za-z]):/ \/cygdrive\/\1/g' ${@:.o=.d} ; \
	fi
endif

#
# The rule for creating an object library.
#
${COMPILER}${SUFFIX}/%.a:
	@if [ 'x${VERBOSE}' = x ];     \
	 then                          \
	     echo "  AR    ${@}";      \
	 else                          \
	     echo ${AR} -cr ${@} ${^}; \
	 fi
	@${AR} -cr ${@} ${^}

#
# The rule for linking the application.
#
${COMPILER}${SUFFIX}/%.axf:
	@if [ 'x${SCATTERgcc_${notdir ${@:.axf=}}}' = x ];                    \
	 then                                                                 \
	     ldname="${ROOT}/gcc/standalone.ld";                              \
	 else                                                                 \
	     ldname="${SCATTERgcc_${notdir ${@:.axf=}}}";                     \
	 fi;                                                                  \
	 if [ 'x${VERBOSE}' = x ];                                            \
	 then                                                                 \
	     echo "  LD    ${@} ${LNK_SCP}";                                  \
	 else                                                                 \
	     echo ${LD} -T $${ldname}                                         \
	          --entry ${ENTRY_${notdir ${@:.axf=}}}                       \
	          ${LDFLAGSgcc_${notdir ${@:.axf=}}}                          \
	          ${LDFLAGS} -o ${@} $(filter %.o %.a, ${^})                  \
	          '${LIBM}' '${LIBC}' '${LIBGCC}';                            \
	 fi;                                                                  \
	${LD} -T $${ldname}                                                   \
	      --entry ${ENTRY_${notdir ${@:.axf=}}}                           \
	      ${LDFLAGSgcc_${notdir ${@:.axf=}}}                              \
	      ${LDFLAGS} -o ${@} $(filter %.o %.a, ${^})                      \
	      '${LIBM}' '${LIBC}' '${LIBGCC}'
	@${OBJCOPY} -O binary ${@} ${@:.axf=.bin}
endif








#
# Where to find header files that do not live in the source directory.
#
IPATH=/home/ed/git_repos/embedded/tivaware/

#
# The default rule, which causes the blinky example to be built.
#
all: ${COMPILER}
all: ${COMPILER}/lab_02.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the blinky example.
#
${COMPILER}/lab_02.axf: ${COMPILER}/main.o
${COMPILER}/lab_02.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/lab_02.axf: TM4C123Gx.ld
SCATTERgcc_lab_02=TM4C123Gx.ld
ENTRY_lab_02=ResetISR

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
