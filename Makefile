ifeq ($(NEWLIBDIR),)
$error("Must set NEWLIBDIR to point to newlib sources")
endif

ifeq ($(TARGET),)
CC=gcc
OUTDIR=x86
OBJCOPY=objcopy
READELF=readelf
else
ifeq ($(TOOLS),)
$error("Must set TOOLS to point to toolchain root")
endif
CC=$(TOOLS)/bin/$(TARGET)-gcc
OBJCOPY=$(TOOLS)/bin/$(TARGET)-objcopy
READELF=$(TOOLS)/bin/$(TARGET)-readelf
OUTDIR=$(TARGET)

INC=$(TOOLS)/$(TARGET)/include

ifeq (, $(findstring linux, $(TARGET)))
INCFLAGS+=-I nmipsinc  \
	-I $(TOOLS)/include -I $(TOOLS)/$(TARGET)/include -I $(subst elf,linux-gnu,$(INC))
SRCCOM=src/gettimeofday.c
CFLAGS+=-ffreestanding  -gdwarf-2 -msoft-float #-mdsp
else # linux
ifneq (, $(findstring nanomips, $(TARGET)))
CFLAGS+=-static -msoft-float -gdwarf-2
else
CFLAGS+=-static -mno-shared -mhard-float -gdwarf-2
endif
MIPS_ONLY=$(OUTDIR)/bench-memcpy_def.uImage
SRCCOM=src/gettimeofday.c
endif # linux

ifeq ($(ENDIAN),)
CFLAGS+=-EL
else
CFLAGS+=-$(ENDIAN)
endif

ifneq (, $(findstring nanomips, $(TARGET)))
MIPS_ONLY=$(OUTDIR)/bench-memcpy_def.uImage 	\
	$(OUTDIR)/bench-memset-large_def.uImage \
	$(OUTDIR)/bench-imemcpy_mi4.uImage \
	$(OUTDIR)/bench-imemcpy_def.uImage \
	$(OUTDIR)/bench-imemcpy_mi16.uImage \
	$(OUTDIR)/bench-imemcpy_multi.uImage \
	$(OUTDIR)/test-strcmp_O2.uImage \
	$(OUTDIR)/test-strcmp_Os.uImage \
	$(OUTDIR)/bench-switch_case5_O2.uImage \
	$(OUTDIR)/bench-switch_case6_O2.uImage \
	$(OUTDIR)/bench-switch_case7_O2.uImage \
	$(OUTDIR)/bench-switch_case8_O2.uImage \
	$(OUTDIR)/bench-switch_case9_O2.uImage \
	$(OUTDIR)/bench-switch_case10_O2.uImage \
	$(OUTDIR)/bench-switch_case11_O2.uImage \
	$(OUTDIR)/bench-switch_case12_O2.uImage \
	$(OUTDIR)/bench-switch_case13_O2.uImage \
	$(OUTDIR)/bench-switch_case5_Os.uImage \
	$(OUTDIR)/bench-switch_case6_Os.uImage \
	$(OUTDIR)/bench-switch_case7_Os.uImage \
	$(OUTDIR)/bench-switch_case8_Os.uImage \
	$(OUTDIR)/bench-switch_case9_Os.uImage \
	$(OUTDIR)/bench-switch_case10_Os.uImage \
	$(OUTDIR)/bench-switch_case11_Os.uImage \
	$(OUTDIR)/bench-switch_case12_Os.uImage \
	$(OUTDIR)/bench-switch_case13_Os.uImage \
	$(OUTDIR)/bench-jumpopt1.uImage \
	$(OUTDIR)/bench-jumpopt2.uImage \
	$(OUTDIR)/bench-jumpopt3.uImage \
	$(OUTDIR)/bench-jumpopt4.uImage \
	$(OUTDIR)/bench-jumpopt5.uImage \
	$(OUTDIR)/bench-jumpopt6.uImage \
	$(OUTDIR)/bench-jumpopt7.uImage \
	$(OUTDIR)/bench-jumpopt8.uImage \
	$(OUTDIR)/bench-jumpopt9.uImage \
	$(OUTDIR)/bench-jumpopt10.uImage \
	$(OUTDIR)/bench-jumpopt11.uImage \
	$(OUTDIR)/bench-jumpopt12.uImage \
	$(OUTDIR)/bench-jumpopt13.uImage \
	$(OUTDIR)/bench-jumpopt14.uImage \
	$(OUTDIR)/bench-jumpopt15.uImage \
	$(OUTDIR)/bench-jumpopt16.uImage \
	$(OUTDIR)/bench-jumpopt17.uImage \
	$(OUTDIR)/bench-jumpopt18.uImage \
	$(OUTDIR)/bench-jumpopt19.uImage \
	$(OUTDIR)/bench-jumpopt20.uImage \
	$(OUTDIR)/bench-jumpopt21.uImage \
	$(OUTDIR)/bench-jumpopt22.uImage \
	$(OUTDIR)/bench-jumpopt23.uImage \
	$(OUTDIR)/bench-jumpopt24.uImage \
	$(OUTDIR)/bench-strcmp_asm.uImage \
	$(OUTDIR)/bench-strcmp_asm2.uImage \
	$(OUTDIR)/bench-strlen-mips-dsp.uImage \
	$(OUTDIR)/bench-strlen-mips4-dsp.uImage \
	$(OUTDIR)/bench-strcpy-mips-dsp.uImage \
	$(OUTDIR)/bench-memset_def.uImage \
	$(OUTDIR)/bench-memcpy_asm.uImage \
	$(OUTDIR)/bench-strcmp_Os.uImage
#	$(OUTDIR)/bench-strcmp_asm.uImage

CFLAGS+=
else # nanomips
MIPS_ONLY+=$(OUTDIR)/bench-memcpy_def.uImage $(OUTDIR)/bench-memset_def.uImage \
	$(OUTDIR)/bench-memset-large_def.uImage \
	$(OUTDIR)/test-strcmp_O2.uImage \
	$(OUTDIR)/bench-strcmp_Os.uImage \
	$(OUTDIR)/bench-strlen-mips-dsp.uImage \
	$(OUTDIR)/bench-strlen-mips4-dsp.uImage \
	$(OUTDIR)/bench-strcpy-mips-dsp.uImage \
	$(OUTDIR)/bench-strcmp_asm.uImage \
	$(OUTDIR)/bench-strcmp_asm2.uImage

#	$(OUTDIR)/bench-memcpy_asm.uImage
#	$(OUTDIR)/bench-strcmp_asm.uImage \
#	$(OUTDIR)/test-strcmp_asm.uImage \
#	$(OUTDIR)/bench-strcmp_asm2.uImage
endif
endif

ifneq (,$(CORE))
ifeq (i6400,$(CORE))
CFLAGS+=-march=$(CORE)
endif # i6400
CFLAGS+=-mtune=$(CORE)
endif

ifneq (,$(ARCH))
CFLAGS+=-march=$(ARCH)
endif

ifeq (, $(findstring linux, $(TARGET)))
 ifeq ($(ABI),n64)
  ifeq ($(SIM),1)
   LDFLAGS=-Tmti64_64.ld
  else # sim
   LDFLAGS=-Tuhi64_64.ld
 endif
else ifeq ($(ABI),n32)
  ifeq ($(SIM),1)
   LDFLAGS=-Tmti64_n32.ld
  else
   LDFLAGS=-Tuhi64_n32.ld
 endif
else
 ifeq (, $(findstring nanomips, $(TARGET)))
  ifneq ($(TARGET),)
    ifeq ($(SIM),1)
     LDFLAGS=-Tmti32.ld
    else
     LDFLAGS=-Tuhi32.ld
   endif
  endif
 else
  ifeq ($(SIM),1)
   LDFLAGS=-Tmti32.ld
  else
   LDFLAGS=-Tuhi32.ld
  endif
 endif
endif
endif

ifeq ($(ABI),n64)
 CFLAGS+=-mabi=64
else ifeq ($(ABI),n32)
 CFLAGS+=-mabi=n32
else
 ifeq (, $(findstring nanomips, $(TARGET)))
  ifneq ($(TARGET),)
   CFLAGS+=-mabi=32
  endif
 endif
endif

ifneq ($(SIM),1)
LDFLAGS += -Wl,--defsym,__memory_size=32M
endif

#LDFLAGS += -Wl,-x

CFLAGS+=-D_POSIX_TIMERS -D_POSIX_CPUTIME -DM_PERTURB=1 $(INCFLAGS) -save-temps -ffunction-sections


#ifeq ($(TARGET),mips-mti-linux-gnu)
#BAREMETAL_ONLY=	$(OUTDIR)/bench-strlen-newlib.uImage \
#	$(OUTDIR)/bench-strcpy-newlib.uImage
#endif

BENCHMARKS=$(OUTDIR)/bench-memcpy_Os.uImage \
	$(OUTDIR)/bench-memcpy_O2.uImage \
	$(OUTDIR)/bench-memset_Os.uImage \
	$(OUTDIR)/bench-memset_O2.uImage \
	$(OUTDIR)/bench-strcmp_def.uImage \
	$(OUTDIR)/bench-memcpy-large_Os.uImage \
	$(OUTDIR)/bench-memcpy-large_O2.uImage \
	$(OUTDIR)/bench-memset-large_Os.uImage \
	$(OUTDIR)/bench-memset-large_O2.uImage \
	$(OUTDIR)/bench-imemcpy_def.uImage \
	$(MIPS_ONLY) \
	$(OUTDIR)/bench-strcmp_O2.uImage \
	$(OUTDIR)/bench-strcmp_Os.uImage \
	$(OUTDIR)/bench-strlen.uImage \
	$(OUTDIR)/bench-strlen-musl.uImage \
	$(OUTDIR)/bench-strlen-mips.uImage \
	$(OUTDIR)/bench-strlen-mips4.uImage \
	$(OUTDIR)/bench-strcpy.uImage \
	$(OUTDIR)/bench-strcpy-mips.uImage \
	$(OUTDIR)/bench-strcpy-musl.uImage \
	$(BAREMETAL_ONLY)

.PRECIOUS: ${BENCHMARKS:.uImage=.elf}

ifeq ($(SIM),1)
all: $(OUTDIR) ${BENCHMARKS:.uImage=.elf}
else
ifneq (, $(findstring linux, $(TARGET)))
all: $(OUTDIR) ${BENCHMARKS:.uImage=.elf}
else
all: $(OUTDIR) ${BENCHMARKS}
endif
endif

ifeq ($(ABI),n64)
$(OUTDIR)/%.uImage: $(OUTDIR)/%.bin
	mkimage -n $(@:.uImage=.elf) \
		-A mips \
		-O u-boot \
		-C none \
		-T standalone \
		-a $(shell $(READELF) -Wl $(@:.uImage=.elf) | grep -m1 'LOAD.*R.E' | awk '{print $$3}') \
		-e $(shell $(READELF) -h $(@:.uImage=.elf) | grep 'Entry point' | awk '{print $$4}') \
		-d $< $@
else
$(OUTDIR)/%.uImage: $(OUTDIR)/%.bin
	mkimage -n $(@:.uImage=.elf) \
		-A mips \
		-O u-boot \
		-C none \
		-T standalone \
		-a $(shell $(READELF) -Wl $(@:.uImage=.elf) | grep -m1 'LOAD.*R.E' | awk '{print $$3}') \
		-e $(shell $(READELF) -h $(@:.uImage=.elf) | grep 'Entry point' | awk '{print $$4}') \
		-d $< $@
endif

$(OUTDIR)/%.bin: $(OUTDIR)/%.elf
	$(OBJCOPY) -O binary \
		--set-section-flags .bss=alloc,load,contents \
		--set-section-flags .sbss=alloc,load,contents \
		--set-section-flags .startdata=alloc,load,contents \
		$< $@

# Force optimization level to test inline memcpy
ifneq (,$(TARGET))
$(OUTDIR)/bench-imemcpy_mi16.elf:CFLAGS+= -mmemcpy-interleave=16 -Os  -mno-memcpy
$(OUTDIR)/bench-imemcpy_mi4.elf:CFLAGS+= -mmemcpy-interleave=4 -Os  -mno-memcpy
$(OUTDIR)/bench-imemcpy_multi.elf:CFLAGS+= -mmulti-memcpy -Os -mno-memcpy
$(OUTDIR)/bench-imemcpy_def.elf:CFLAGS+= -Os -mno-memcpy
endif

$(OUTDIR):
	mkdir $@

INCM=$(subst linux-gnu,elf,$(TOOLS)/$(TARGET)/include/mips)

%.o: src/%.S
	$(CC) $(CFLAGS) -c -I $(TOOLS)/$(TARGET)/include/mips -I $(INCM) $^ -o $@

%_Os.o: $(NEWLIBDIR)/%.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

%_Os.o: src/%-c.c
	$(CC) $(CFLAGS) -Os -c $^ -o $@

%_O2.o: $(NEWLIBDIR)/%.c
	$(CC) $(CFLAGS) -O2 -c $^ -o $@

%_O2.o: src/%-c.c
	$(CC) $(CFLAGS) -O2 -c $^ -o $@


strcmp_asm2.o: $(NEWLIBDIR)/strcmp.S
	$(CC)  $(CFLAGS) -O2 -c -I $(TOOLS)/$(TARGET)/include/mips  -I $(INCM) $^ -o $@

strcmp_asm.o: src/strcmp_asm.S
	$(CC)  $(CFLAGS) -Os -c -I $(TOOLS)/$(TARGET)/include/mips  -I $(INCM) $^ -o $@

%_asm.o: src/%-asm.s
	$(CC) $(CFLAGS) -c -I $(TOOLS)/$(TARGET)/include/mips  -I $(INCM) $^ -o $@

%_asm.o: $(NEWLIBDIR)/%.S
	$(CC) $(CFLAGS) -c -I $(TOOLS)/$(TARGET)/include/mips  -I $(INCM) $^ -o $@

%_asm.o: $(NEWLIBDIR)/%.S
	$(CC) $(CFLAGS) -c -I $(TOOLS)/$(TARGET)/include/mips  -I $(INCM) $^ -o $@

$(OUTDIR)/bench-%_asm.elf: src/bench-%.c %_asm.o $(SRCCOM)
	$(CC) $(CFLAGS) -Os $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_asm2.elf: src/bench-%.c %_asm2.o $(SRCCOM)
	$(CC) $(CFLAGS) -Os $^ -o $@  $(LDFLAGS)

$(OUTDIR)/test-%_asm.elf: src/test-%.c %_asm.o $(SRCCOM)
	$(CC) $(CFLAGS) -Os $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_def.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-switch_case3_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=3
$(OUTDIR)/bench-switch_case4_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=4
$(OUTDIR)/bench-switch_case5_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=5
$(OUTDIR)/bench-switch_case6_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=6
$(OUTDIR)/bench-switch_case7_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=7
$(OUTDIR)/bench-switch_case8_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=8
$(OUTDIR)/bench-switch_case9_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=9
$(OUTDIR)/bench-switch_case10_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=10
$(OUTDIR)/bench-switch_case11_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=11
$(OUTDIR)/bench-switch_case12_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=12
$(OUTDIR)/bench-switch_case13_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=13
$(OUTDIR)/bench-switch_case14_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=14
$(OUTDIR)/bench-switch_case15_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=15
$(OUTDIR)/bench-switch_case16_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=16
$(OUTDIR)/bench-switch_case17_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=17
$(OUTDIR)/bench-switch_case18_O2.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=18
$(OUTDIR)/bench-switch_case3_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=3
$(OUTDIR)/bench-switch_case4_Os.eyyyyyylf:CFLAGS+= -falign-functions=32 --param case-values-threshold=4
$(OUTDIR)/bench-switch_case5_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=5
$(OUTDIR)/bench-switch_case6_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=6
$(OUTDIR)/bench-switch_case7_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=7
$(OUTDIR)/bench-switch_case8_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=8
$(OUTDIR)/bench-switch_case9_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=9
$(OUTDIR)/bench-switch_case10_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=10
$(OUTDIR)/bench-switch_case11_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=11
$(OUTDIR)/bench-switch_case12_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=12
$(OUTDIR)/bench-switch_case13_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=13
$(OUTDIR)/bench-switch_case14_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=14
$(OUTDIR)/bench-switch_case15_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=15
$(OUTDIR)/bench-switch_case16_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=16
$(OUTDIR)/bench-switch_case17_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=17
$(OUTDIR)/bench-switch_case18_Os.elf:CFLAGS+= -falign-functions=32 --param case-values-threshold=18


JCFLAGS1=
JCFLAGS2= -mno-pcrel -mcmodel=auto
JCFLAGS3= -mpcrel -mcmodel=medium
JCFLAGS4= -mno-pcrel -mcmodel=medium
JCFLAGS5= -mpcrel -mcmodel=large
JCFLAGS6= -mno-pcrel -mcmodel=large
JCFLAGS=
$(OUTDIR)/bench-jumpopt1.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS1) -O2 -DTEST_CFLAGS="\"$(JCFLAGS1)\"" 
$(OUTDIR)/bench-jumpopt2.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS2) -O2 -DTEST_CFLAGS="\"$(JCFLAGS2)\""
$(OUTDIR)/bench-jumpopt3.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS3) -O2 -DTEST_CFLAGS="\"$(JCFLAGS3)\""
$(OUTDIR)/bench-jumpopt4.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS4) -O2 -DTEST_CFLAGS="\"$(JCFLAGS4)\""
$(OUTDIR)/bench-jumpopt5.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS5) -O2 -DTEST_CFLAGS="\"$(JCFLAGS5)\""
$(OUTDIR)/bench-jumpopt6.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS6) -O2 -DTEST_CFLAGS="\"$(JCFLAGS6)\""
$(OUTDIR)/bench-jumpopt7.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS1) -Os -DTEST_CFLAGS="\"$(JCFLAGS1)\"" 
$(OUTDIR)/bench-jumpopt8.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS2) -Os -DTEST_CFLAGS="\"$(JCFLAGS2)\""
$(OUTDIR)/bench-jumpopt9.elf:CFLAGS+= $(JCFLAGS)  $(JCFLAGS3) -Os -DTEST_CFLAGS="\"$(JCFLAGS3)\"" 
$(OUTDIR)/bench-jumpopt10.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS4) -Os -DTEST_CFLAGS="\"$(JCFLAGS4)\"" 
$(OUTDIR)/bench-jumpopt11.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS5) -Os -DTEST_CFLAGS="\"$(JCFLAGS5)\"" 
$(OUTDIR)/bench-jumpopt12.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS6) -Os -DTEST_CFLAGS="\"$(JCFLAGS6)\""
$(OUTDIR)/bench-jumpopt13.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS1) -O2 -DTEST_CFLAGS="\"$(JCFLAGS1)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt14.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS2) -O2 -DTEST_CFLAGS="\"$(JCFLAGS2)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt15.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS3) -O2 -DTEST_CFLAGS="\"$(JCFLAGS3)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt16.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS4) -O2 -DTEST_CFLAGS="\"$(JCFLAGS4)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt17.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS5) -O2 -DTEST_CFLAGS="\"$(JCFLAGS5)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt18.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS6) -O2 -DTEST_CFLAGS="\"$(JCFLAGS6)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt19.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS1) -Os -DTEST_CFLAGS="\"$(JCFLAGS1)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt20.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS2) -Os -DTEST_CFLAGS="\"$(JCFLAGS2)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt21.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS3) -Os -DTEST_CFLAGS="\"$(JCFLAGS3)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt22.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS4) -Os -DTEST_CFLAGS="\"$(JCFLAGS4)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt23.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS5) -Os -DTEST_CFLAGS="\"$(JCFLAGS5)\"" -mjump-table-opt
$(OUTDIR)/bench-jumpopt24.elf:CFLAGS+= $(JCFLAGS) $(JCFLAGS6) -Os -DTEST_CFLAGS="\"$(JCFLAGS6)\"" -mjump-table-opt

$(OUTDIR)/bench-switch_case%_O2.elf: src/bench-switch.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -O2 -o $@  $(LDFLAGS)

$(OUTDIR)/bench-tmp1.elf: src/bench-test.c $(SRCCOM)
	$(CC) $(CFLAGS) -Os $^ -o $@  $(LDFLAGS)
$(OUTDIR)/bench-tmp2.elf: src/bench-test.c $(SRCCOM)
	$(CC) $(CFLAGS) -O2 $^ -o $@  $(LDFLAGS)
$(OUTDIR)/bench-tmp3.elf: src/bench-test.c $(SRCCOM)
	$(CC) $(CFLAGS) -O3 $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-jumpopt%.elf: src/bench-jumpopt.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-switch_case%_Os.elf: src/bench-switch.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -Os -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_mi16.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_mi4.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_multi.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%.elf: src/bench-%.c $(SRCCOM) %.o
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_Os.elf: src/bench-%.c $(SRCCOM) %_Os.o
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_O2.elf: src/bench-%.c $(SRCCOM) %_O2.o
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

$(OUTDIR)/test-%_O2.elf: src/test-%.c $(SRCCOM) %_O2.o
	$(CC) $(CFLAGS) -O2 $^ -o $@  $(LDFLAGS)

$(OUTDIR)/test-%_Os.elf: src/test-%.c $(SRCCOM) %_Os.o
	$(CC) $(CFLAGS) -Os $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%_O3.elf: src/bench-%.c $(SRCCOM) %_O3.o
	$(CC) $(CFLAGS)  $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%-large_def.elf: src/bench-%.c $(SRCCOM)
	$(CC) $(CFLAGS) -DLARGEBUF $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%-large_Os.elf: src/bench-%.c $(SRCCOM) %_Os.o
	$(CC) $(CFLAGS) -DLARGEBUF $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%-large_O2.elf: src/bench-%.c $(SRCCOM) %_O2.o
	$(CC) $(CFLAGS) -DLARGEBUF $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-%-large_O3.elf: src/bench-%.c $(SRCCOM) %_O3.o
	$(CC) $(CFLAGS) -DLARGEBUF  $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-strlen-%.elf: src/bench-strlen.c $(SRCCOM) src/strlen-%.c
	$(CC) $(CFLAGS) -O2 $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-strlen-%-dsp.elf: src/bench-strlen.c $(SRCCOM) src/strlen-%.c
	$(CC) $(CFLAGS) -O2 -mdsp $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-strcpy-%.elf: src/bench-strcpy.c $(SRCCOM) src/strcpy-%.c
	$(CC) $(CFLAGS) -O2 $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-strcpy-%-dsp.elf: src/bench-strcpy.c $(SRCCOM) src/strcpy-%.c
	$(CC) $(CFLAGS) -O2 -mdsp $^ -o $@  $(LDFLAGS)

$(OUTDIR)/bench-strlen-%.elf: src/bench-strlen.c $(SRCCOM) src/strlen-%.s
	$(CC) $(CFLAGS) $^ -o $@  $(LDFLAGS)

.PHONY: clean
clean:
	rm -rf $(OUTDIR) *.o *.s *.i
