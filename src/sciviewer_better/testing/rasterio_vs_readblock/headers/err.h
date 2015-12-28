#ifndef __ERR_H_
#define __ERR_H_
#include <stdlib.h>
#include <stdio.h>
#define FAILIF(var,badresult,descr) { if(var == badresult) { puts(descr); exit(EXIT_FAILURE);}}

#endif
