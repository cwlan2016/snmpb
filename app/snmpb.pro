SOURCES	+= ../snmp++/src/address.cpp \
	../snmp++/src/asn1.cpp \
	../snmp++/src/auth_priv.cpp \
	../snmp++/src/counter.cpp \
	../snmp++/src/ctr64.cpp \
	../snmp++/src/eventlist.cpp \
	../snmp++/src/eventlistholder.cpp \
	../snmp++/src/gauge.cpp \
	../snmp++/src/idea.cpp \
	../snmp++/src/integer.cpp \
	../snmp++/src/log.cpp \
	../snmp++/src/md5c.cpp \
	../snmp++/src/mp_v3.cpp \
	../snmp++/src/msec.cpp \
	../snmp++/src/msgqueue.cpp \
	../snmp++/src/notifyqueue.cpp \
	../snmp++/src/octet.cpp \
	../snmp++/src/oid.cpp \
	../snmp++/src/pdu.cpp \
	../snmp++/src/reentrant.cpp \
	../snmp++/src/sha.cpp \
	../snmp++/src/snmpmsg.cpp \
	../snmp++/src/target.cpp \
	../snmp++/src/timetick.cpp \
	../snmp++/src/usm_v3.cpp \
	../snmp++/src/uxsnmp.cpp \
	../snmp++/src/v3.cpp \
	../snmp++/src/vb.cpp \
	main.cpp \
	mibnode.cpp \
	mibview.cpp \
	mibmodule.cpp \
	agent.cpp \
	trap.cpp \
	graph.cpp \
	configfiles.cpp
HEADERS	+= mibnode.h \
	mibview.h \
	mibmodule.h \
	agent.h \
	trap.h \
	graph.h \
	configfiles.h
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
}
FORMS	= mainw.ui
TEMPLATE	=app
CONFIG	+= qt warn_on
win32:CONFIG += release
win32:DEFINES	+= WIN32
win32:QMAKE_CXXFLAGS += -mno-cygwin
win32:QMAKE_LFLAGS += -mno-cygwin
win32:RC_FILE = snmpb.rc
RESOURCES     = snmpb.qrc
INCLUDEPATH	+= ../snmp++/include ../libtomcrypt/src/headers ../libsmi/lib ../qwt/include
LIBS	+= -L. -L../libtomcrypt -L../libsmi/lib/.libs -L../qwt/lib -lsmi -ltomcrypt -lqwt
win32:LIBS	+= -lws2_32 -L../libsmi/win
LANGUAGE	= C++
#The following line was inserted by qt3to4
QT +=  qt3support 

