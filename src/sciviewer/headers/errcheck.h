#ifndef __ERRCHECK_H__
#define __ERRCHECK_H__

#define FAILIF(var,expected,message) if(var == expected) { puts(message); exit(EXIT_FAILURE); }

#endif
