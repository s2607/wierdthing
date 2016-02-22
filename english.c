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
#include <string.h>
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
char *bes[]={"","","", "am","","will","where","","will",0};
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
char *be(int n,int ing){
	char *s=bes[n];
	if(ing==ING&&besing[n])
		s=besing[n];
	if(ing==ED&&besed[n])
		s=besed[n];
	return s;
}
int num(char *a,char **wl){
	int i=0;
	for(i=0;*(wl+i);i++){
		if(!strcmp(a,*(wl+i))){
			return i;
		}
	}
	return -1;
}
int isbeing(phrase *r) {
	return (num(r->s,bes)>0||num(r->s,besed)>0||num(r->s,besing)>0);

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
int ising(char *s){
	if(strstr(s,"ed"))
		return ED;
	if(strstr(s,"ing"))
		return ING;
	return 0;
}
void correct(phrase *r) {
	int rpn=num(r->s,pn);
	phrase *verb=r->parts[VERB];
	if(rpn<0)
		rpn=num(r->s,pn)+10;
	if(verb){
		if(rpn<10&&isbeing(verb))
			verb->s=be(rpn,ising(verb->s));
		if(ising(verb->s))
			modify(verb,be(rpn,ising(verb->s)));
	}
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
		if(*(s->m+1)>'a')
			*(s->m+1)=*(s->m+1)-('a'-'A');
	//yyj	sm_appendstr(s,".");
	}else {
		sm_appendstr(s,towords(subj->parts[MODIFY])); 
		sm_appendstr(s," ");
		sm_appendstr(s,subj->s);
	}
	return sm_dumpstr(s);

}
char *paragraph(phrase **p,int l){
	sm *s=sm_new(5);
	int i=0;
	for(i=0;i<=l;i++){
		sm_appendstr(s,towords(p[i]));
		sm_appendstr(s,".");
	}
	return sm_dumpstr(s);
}
char *experience() {
	phrase *p[3];
	p[0]=sentence("I","computers","programed",NULL);
	p[0]->parts[SUBP]=sentence("","years","for",NULL);
	modify(((phrase *)p[0]->parts[SUBP])->parts[OBJ],"9");
	p[1]=sentence("I",NULL,"worked",NULL);
	p[1]->parts[OBJ]=sentence("","systems","with",NULL);
	modify(((phrase *)p[1]->parts[OBJ])->parts[OBJ],"embeded");
	return paragraph(p,1);

}
char *calling() {
	phrase *p[3];
	p[0]=sentence("I",NULL,"been called",NULL);
	//modify(p[0]->parts[VERB],"feel");
	p[0]->parts[OBJ]=sentence("","automation","to work with",NULL);
	modify(((phrase *)p[0]->parts[OBJ])->parts[OBJ],"agricultural");
	p[1]=sentence("math","me","interests","also");

	return paragraph(p,1);
}
