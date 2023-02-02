HEADERS += \
    $$PWD/aesni/aesni-enc-cbc.h \
    $$PWD/aesni/aesni-enc-ecb.h \
    $$PWD/aesni/aesni-key-exp.h \
    $$PWD/qaesencryption.h

SOURCES += \
    $$PWD/qaesencryption.cpp

#DEFINES += USE_INTEL_AES_IF_AVAILABLE
#QMAKE_CXXFLAGS += -maes
