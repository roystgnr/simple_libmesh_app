# Get the path to the libmesh_config script
libmesh_config := $(LIBMESH_DIR)/bin/libmesh-config           # installed version
ifeq ($(wildcard $(libmesh_config)),)
  libmesh_config := $(LIBMESH_DIR)/contrib/bin/libmesh-config # uninstalled version
endif

# Optimized binary by default
METHOD ?= opt

appname := simple
full_appname := $(appname)-$(METHOD)

# Instead of using Make.common, use libmesh-config to get any libmesh
# make variables we might need.  Be sure to pass METHOD along to libmesh-config
# so that it can use the right one!
CXX      ?= $(shell METHOD=$(METHOD) $(libmesh_config) --cxx)
CC       ?= $(shell METHOD=$(METHOD) $(libmesh_config) --cc)
F77      ?= $(shell METHOD=$(METHOD) $(libmesh_config) --fc)
F90      ?= $(shell METHOD=$(METHOD) $(libmesh_config) --fc)
INCLUDE  := $(shell METHOD=$(METHOD) $(libmesh_config) --include)
CPPFLAGS := $(shell METHOD=$(METHOD) $(libmesh_config) --cppflags)
CXXFLAGS := $(shell METHOD=$(METHOD) $(libmesh_config) --cxxflags)
CFLAGS   := $(shell METHOD=$(METHOD) $(libmesh_config) --cflags)
FFLAGS   := $(shell METHOD=$(METHOD) $(libmesh_config) --fflags)
LIBS     := $(shell METHOD=$(METHOD) $(libmesh_config) --libs)
HOST     := $(shell METHOD=$(METHOD) $(libmesh_config) --host)
LDFLAGS  := $(shell METHOD=$(METHOD) $(libmesh_config) --ldflags)

INCLUDE += -I include

INCLUDE += -I contrib/mtwist/include

srcfiles := $(shell find src -name "*.C")
objects	 := $(patsubst %.C, %-$(METHOD).o, $(srcfiles))

all: $(full_appname)

$(appname)-opt: $(objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(INCLUDE) -o $(full_appname) $(objects) $(LDLIBS)

$(appname)-dbg: $(objects)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(INCLUDE) -o $(full_appname) $(objects) $(LDLIBS)

%-opt.o: %.C
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(INCLUDE) -MMD -MP -MF $@.d -MT $@ $< -o $@ $(LDLIBS)

%-dbg.o: %.C
	$(CXX) -c $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) $(LIBS) $(INCLUDE) -MMD -MP -MF $@.d -MT $@ $< -o $@ $(LDLIBS)

clean:
	rm -f *.o
	rm -f $(objects)
	rm -f $(full_appname)
	rm -rf *.d
	rm -f *~*

-include $(patsubst %, %-$(METHOD).o.d, $(basename $(srcfiles)))
