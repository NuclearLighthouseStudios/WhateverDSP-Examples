TARGET = reverb
BOARD = fxdsp

include ../WhateverDSP/libwdsp.mk

OBJECTS = allpass.o comb.o reverb.o revmodel.o reverb.o

allpass.o: allpass.cpp allpass.hpp denormals.h
comb.o: comb.cpp comb.hpp denormals.h
reverb.o: reverb.cpp revmodel.hpp comb.hpp denormals.h allpass.hpp tuning.h
revmodel.o: revmodel.cpp revmodel.hpp comb.hpp denormals.h allpass.hpp tuning.h

$(TARGET): $(OBJECTS)

.PHONY: clean
clean:
	rm -f $(OBJECTS)