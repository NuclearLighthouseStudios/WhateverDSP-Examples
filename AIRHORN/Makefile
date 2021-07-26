TARGET = airhorn
BOARD = wdsp-dev

include ../WhateverDSP/libwdsp.mk

$(TARGET): airhorn.h

airhorn.h: airhorn.raw
	xxd -i $< | sed -e '1 s/^/const /' > $@

clean:
	rm -f airhorn.h