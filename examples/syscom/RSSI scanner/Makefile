#DEFINES=PROJECT_CONF_H=\"project-conf.h\"

ifndef TARGET
  TARGET = z1
endif

# Disable all networking layer 3 upwards 
MAKE_NET=MAKE_NET_NULLNET

# use a custom MAC driver: sensniff_mac_driver
##MAKE_MAC = MAKE_MAC_OTHER

CONTIKI = ../../..

all: $(CONTIKI_PROJECT)

%.class: %.java
	javac $(basename $<).java

viewrssi: ViewRSSI.class
	make login | java ViewRSSI

include $(CONTIKI)/Makefile.include
