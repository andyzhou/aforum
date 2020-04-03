#APXS = apxs
CC = gcc
#INCDIR = `$(APXS) -q INCLUDEDIR`
#CFLAGS = `$(APXS) -q CFLAGS`
SHELL = /bin/sh

SRCDIR=../../../..
EXTRA_CFLAGS=`$(SRCDIR)/apaci`
OSDIR=$(SRCDIR)/os/unix
#INCDIR=$(SRCDIR)/include
INCDIR=$(SRCDIR)/include -I$(SRCDIR)/srclib/apr/include -I$(SRCDIR)/srclib/apr-util/include
INCLUDES0=-I$(OSDIR) -I$(INCDIR)

FULLEXT = libtpllib
BASEEXT = libtpllib
DLBASE = $(BASEEXT)
OBJ_EXT = .o
LIB_EXT = .a
RANLIB = :
AR = ar
AR_STATIC_ARGS = cr
CHMOD = chmod
PERM_RWX = 755
RM_F = rm -f
RM_RF = rm -rf
CCCMD = $(CC)
NOOP = $(SHELL) -c true

.c$(OBJ_EXT):
	$(CCCMD) -c $(CFLAGS) -I$(INCDIR) $(DEFINE) $*.c

OBJECT = tpllib$(OBJ_EXT)
LDFROM = $(OBJECT)

INST_STATIC  = $(BASEEXT)$(LIB_EXT)

all::Makefile $(INST_STATIC)
	@$(NOOP)

static :: Makefile $(INST_STATIC)
	@$(NOOP)

$(INST_STATIC): $(OBJECT)
	$(RM_RF) $@
	$(AR) $(AR_STATIC_ARGS) $@ $(OBJECT) && $(RANLIB) $@
	$(CHMOD) $(PERM_RWX) $@

clean ::
	$(RM_RF) $(INST_STATIC) *.o

