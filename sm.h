//Copyright FEB 2016 Stephen William Wiley WTFPL
typedef struct sm {
	//stretchy mem
	char *m;
	int l;
	int i;
}sm;
void dieif(int c, char *m);
void sm_init(sm *m,int l) ;
void sm_stretch(sm *m, int ld) ;
void sm_append(sm *m,char *d, int l) ;
void sm_appendstr(sm *m, char *s) ;
void sm_appendstrf(sm *m, char *s) ;
void sm_free(sm *m) ;
char *sm_extract(sm *m);
char *sm_dump(sm *m) ;
char *sm_dumpstr(sm *m) ;//ensure zero at en;
sm *sm_new(int l);
sm *sm_slice(sm *s,int a, int b) ;
//really should go in an english.h
char *experience();
char *calling();
