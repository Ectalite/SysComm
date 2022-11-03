CONTIKI_PROJECT = testSht11
all: $(CONTIKI_PROJECT)

CONTIKI = ../../..

ifeq ($(TARGET),z1)
	MODULES += arch/dev/sensor/sht11
	MODULES += arch/platform/z1/dev
endif

include $(CONTIKI)/Makefile.include

ifeq ($(TARGET),z1)
	# Enable to pull-in Z1SP specific test/source files
	ZOLERTIA_Z1SP=0
endif



