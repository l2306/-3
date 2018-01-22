#include <stdio.h>
#include <string.h>
#include <errno.h>


#ifndef DEBUG
#define DEBUG printf("erro %s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
#else
#define DEBUG
#endif

#ifndef RUNHERE
#define RUNHERE printf("%s %s %d\n",__FILE__,__FUNCTION__,__LINE__);
#else
#define RUNHERE 
#endif
