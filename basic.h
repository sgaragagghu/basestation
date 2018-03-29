//basic.h
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include "utils.h"
#include "rvgs.h"
#include "rngs.h"


// ########## settings ################

#define SAMPLETIME 20
#define ARRIVALMEAN1 0.15384615384615
#define ARRIVALMEAN2 0.16
#define SERVICEMEANCLET1 2.22222222222222
#define SERVICEMEANCLET2 3.7037037037037
#define SERVICEMEANC1 4.0
#define SERVICEMEANC2 4.54545454545455
#define MIGRATIONDELAY 0.8
#define SIMULATIONLENGTH 86400.00
#define SEED 123456789
#define ARRIVALSTREAM1 0
#define ARRIVALSTREAM2 2
#define LEAVESTREAMCLET1 55
#define LEAVESTREAMCLET2 57
#define LEAVESTREAMC1 50
#define LEAVESTREAMC2 53
#define MIGRATIONDELAYSTREAM 58
#define ITERATION 1
#define PRINTALL 1
#define FINALVALUEFILES 1
#define STARTTOPRINT 0.00
#define STARTTOCOUNT 86400.00
#define PRINTINTERs 1 //print throughput, interarrival and interdeparture

#define CAPACITY 20
#define THRESHOLD 20

#define VERBOSELIST 0
#define LISTTYPE double
#define LISTTYPE2 char
#define LISTTYPE3 double
#define LISTTYPE4 double
