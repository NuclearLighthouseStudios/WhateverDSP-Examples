TARGET = airhorn
BOARD = wdsp-dev

include ../WhateverDSP/libwdsp.mk

$(TARGET): airhorn.h

# We're using xxd to automatically generate a header from the raw audio
# of the sample. sed is used to add a 'const' specifier so it goes into flash.
airhorn.h: airhorn.raw
	xxd -i $< | sed -e '1 s/^/const /' > $@

clean:
	rm -f airhorn.h