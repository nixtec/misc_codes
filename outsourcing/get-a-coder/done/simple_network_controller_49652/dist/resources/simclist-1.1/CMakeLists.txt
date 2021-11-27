PROJECT(simclist)

# simclist options
OPTION(SIMCLIST_DEBUG
    "Build with debug code and debug symbols enabled"
    OFF)
OPTION(SIMCLIST_THREADING
    "Build with simclist threading enable"
    OFF)

# expand selected options
SET(SIMCCFLAGS "")
IF(SIMCLIST_DEBUG)
    SET(SIMCCFLAGS "${SIMCCFLAGS} -DSIMCLIST_DEBUG")
ENDIF(SIMCLIST_DEBUG)
IF(SIMCLIST_THREADING)
    SET(SIMCCFLAGS "${SIMCCFLAGS} -DSIMCLIST_WITH_THREADS")
ENDIF(SIMCLIST_THREADING)
SET_SOURCE_FILES_PROPERTIES(simclist.c COMPILE_FLAGS "${SIMCCFLAGS}")

# main building stuff
ADD_LIBRARY(simclist SHARED simclist.c)
SET(CMAKE_C_FLAGS "-O2 -Wall")
