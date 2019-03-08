
# FOR HP 64 BIT SYSTEM
#CFLAGS = +DA2.0W +DS2.0 -DSS_64BIT_SERVER
#LFLAGS = -L${ORACLE_HOME}/lib/ -lclntsh `cat ${ORACLE_HOME}/lib/sysliblist` `cat ${ORACLE_HOME}/lib/ldflags`


# FOR HP 32 BIT SYSTEM
#CFLAGS =
#LFLAGS = -L${ORACLE_HOME}/lib/ -lclntsh `cat ${ORACLE_HOME}/lib/sysliblist`


# FOR IBM 64 BIT SYSTEM
CFLAGS = -q64
LFLAGS = -L$(ORACLE_HOME)/lib/ -lclntsh `cat $(ORACLE_HOME)/lib/sysliblist` `cat $(ORACLE_HOME)/lib/ldflags`


#DEFINE = 
DEFINE = -D __INCREMENT_FILE__

ZLIB = /pkg11/src/SCCS/lib/libz.a
#ZLIB = ./libz.a

CC = /usr/bin/xlC
#CC = /usr/bin/cc
COMPILE = ${CC} ${CFLAGS} ${DEFINE}
MAKEEXE = ${CC} ${CFLAGS} ${LFLAGS} ${DEFINE}


STOREOBJ = cmain.o sdetail.o common.o cdata.c increment.o oracall.o
BUILDOBJ = rmain.o rebuild.o common.o cdata.c oracall.o 
ARCHOBJ  = amain.o archive.o common.o

all:	sdetail rebuild archive

sdetail:	${STOREOBJ}
	 	${MAKEEXE} -o $@ ${STOREOBJ}

rebuild:	${BUILDOBJ}
	 	${MAKEEXE} -o $@ ${BUILDOBJ}

archive:	${ARCHOBJ}
	 	${MAKEEXE} -o $@ ${ARCHOBJ} ${ZLIB}

oracall.o:	oracall.pc
		proc oracall.pc
		${COMPILE} -c oracall.c
		rm -rf oracall.c *.lis

.c.o:
	${COMPILE} -c $<

clean:
		rm -f *.o *.lis core

