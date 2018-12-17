#ifndef mod_h
#define mod_h

#include <stdio.h>
#include <mtcp_api.h>
void setconf(void);
void mod(void);
int socket(int,int,int);
int setnonblocking(int);
void destroy_mtcp(void);
#endif /* mod_h */
