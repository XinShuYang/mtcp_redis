#ifndef mod_h
#define mod_h

#include <stdio.h>
#include <mtcp_api.h>
void setconfm(void);
void mod(void);
void testprint(void);
int socket(int,int,int);
int setnonblocking(int);
void destroy_mtcp(void);
void destroy_nf(void);

#endif /* mod_h */
