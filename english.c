//I have been programming computers for 9 years
//sub verb verb verb         obj     ?? adv ??
//there is
//a
//such that
//b
//.
//there is obj and obj
//such that subj verb obj
//.
//there is me, computers
//such that me be program computers.
#include "sm.h"
#include <malloc.h>
#define NUM_SING 1
#define NUM_PLUR 2
#define TENS_PAST 3
#define TENS_PRESNT 4
#define TENS_FUTR 5
#define ING 1
#define ED 2
#define P 3
char *bes[]={"","","", "was","","will","where","","will",0};
char *besing[]={"","","", "have been","am","will be","where","are","will be",0};

char *besed[]={"","","", "have","","will have","where having","are having","will have",0};
char *ings[]={"","ing","ed"};
char *pn[]= {"","","","I","you","its|he|she","we","you","they"};
char *pnp[]={"","","","mine","yours","its|his|hers","ours","yours","theres"};
typedef struct phrase {
	char *s;
	char **m;
	int tense;
	int num;
	int poses;
	int global;
}phrase;
typedef struct cxt {
	phrase *antecedents[2*6];
	int propcount;
	int the;
}cxt;
phrase *mkp(char *s,char *m){
	//TODO:this is shit, fix it
	phrase *r=calloc(sizeof(char),sizeof(phrase));
	r->s=s;
	r->m=calloc(sizeof(char*),2);
	*r->m=m;
	return r;

}

char *be(int n,int tense, int ing){
	char *s=bes[n*tense];
	if(ing==ING&&besing[n*tense])
		s=besing[n*tense];
	if(ing==ED&&besed[n*tense])
		s=besed[n*tense];
	return s;
}

char *prnoun(int n, int person,int gender, int poses) {
	if(poses)
		return pnp[n*person];
	return pn[n*person];

}
char *tolist(char **t) {
	sm *s=sm_new(100);
	int i;
	for(i=0;*(t+i);i++);
	int l=i;
	for(i=0;i<=l;i++){
		sm_appendstr(s,*(t+i));
		if(i<l&& l>1)
			sm_appendstr(s,", ");
		if(i+1==l)
			sm_appendstr(s,"and ");
	}
}
char *expand(phrase *p,int mode) {
	sm *s=sm_new(100);
	if(!mode){
		char *m=tolist(p->m);
		sm_appendstr(s,m);
		sm_appendstr(s," ");
		sm_appendstr(s,p->s);
	}
	if(mode==1){
		sm_appendstr(s,*p->m);
		sm_appendstr(s,p->s);
	}
	return sm_dumpstr(s);
}
int update(phrase *p,cxt *c){
	return 0;
}
char *scentence(phrase *subj,phrase *verb,phrase *obj,cxt *c) {
	sm *s=sm_new(100);
	
	sm_appendstrf(s,expand(subj,update(subj,c)));
	sm_appendstrf(s,expand(verb,0));
	sm_appendstrf(s,expand(obj,update(obj,c)));
	return sm_dumpstr(s);
}
char *experience() {
	cxt c;
	return scentence(mkp("I",""),mkp("program",""),mkp("computers",""),&c);

}
char *calling() {
	cxt c;
	return scentence(mkp("I",""),mkp("know","not"),mkp("calling","mine"),&c);
}


