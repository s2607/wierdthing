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
#define NUM_P 3+2+1
#define MODIFY 2
#define NEXT 1
#define SUBJ 3
#define OBJ 4
#define VERB 5
#define SUBP 6
char *bes[]={"","","", "was","","will","where","","will",0};
char *besing[]={"","","", "have been","am","will be","where","are","will be",0};

char *besed[]={"","","", "have","","will have","where having","are having","will have",0};
char *ings[]={"","ing","ed",0};
char *pn[]= {"","","","I","you","its|he|she","we","you","they",0};
char *pnp[]={"","","","mine","yours","its|his|hers","ours","yours","theres",0};
typedef struct phrase {//new plan: langauge sentences are directed loop heavy graphs
	char *s;
	int tense;
	int num;
	void *parts[NUM_P];//fuck the standards
}phrase;
typedef struct cxt {
	phrase *antecedents[2*6];
	int propcount;
	int the;
}cxt;
phrase *phrase_new(char *s){
	if(s){
		phrase *r= calloc(sizeof(phrase),1);
		r->s=s;
		return r;
	}
	return NULL;
}
void modify(phrase *p,char *s){
	p->parts[MODIFY]=phrase_new(s);

}
void normalize(phrase *r) {
	int i,j;
	for(i=0;i<=NUM_P;i++){
		if(r->parts[i]){
			phrase *c=r->parts[i];
			for(j=0;j<=NUM_P;j++)
				c->parts[j]=r->parts[j];
		}
	}
}
void correct(phrase *r) {
}
phrase *sentence(char *subj,char *obj,char *verb,char *subp){
	phrase *r=phrase_new(subj);
	r->parts[SUBJ]=r;
	r->parts[OBJ]=phrase_new(obj);
	r->parts[VERB]=phrase_new(verb);
	r->parts[SUBP]=phrase_new(subp);
	normalize(r);
	correct(r);
	return r;
}
void phrase_free(phrase *p) {
	if(p->s)
		free(p->s);
	int i;
	for(i=0;i<=NUM_P;i++)
		if(p->parts[i])
			phrase_free((phrase*)(p->parts[i]));
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
char *towords(phrase *subj){
	sm *s=sm_new(1000);
	if(!subj)
		return sm_dumpstr(s);
	if(subj->parts[SUBJ]==(phrase *)subj){
		sm_appendstr(s," ");
		sm_appendstr(s,towords(subj->parts[MODIFY])); 
		sm_appendstr(s,subj->s);
		sm_appendstr(s,towords(subj->parts[VERB]));
		sm_appendstr(s,towords(subj->parts[OBJ]));
		sm_appendstr(s,towords(subj->parts[SUBP]));
	}else {
		sm_appendstr(s," ");
		sm_appendstr(s,towords(subj->parts[MODIFY])); 
		sm_appendstr(s," ");
		sm_appendstr(s,subj->s);
	}
	return sm_dumpstr(s);

}
char *experience() {
	return towords(sentence("I","computers","program",NULL));
}
char *calling() {
	return towords(sentence("I","computers","program",NULL));
}




