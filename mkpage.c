//Copyright FEB 2016 Stephen William Wiley WTFPL
//cc -g -lm -lcurl mkpage.c -o mkpage &&./mkpage
//obviously has libcurl as a dependancy
//sources: libcurl docs, linux programmer's manual (never used memcpy before, needed reminder on realoc and frwite)
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<curl/curl.h>
#include<curl/easy.h>
const char * PIC_URL= "https://avatars3.githubusercontent.com/u/10791820?v=3&s=400"; //TODO: fix url
const char * PIC_FNAME ="avatar.jpg";
char *dbg_context;
typedef struct sm {
	//stretchy mem
	char *m;
	int l;
	int i;
}sm;
void dieif(int c, char *m);
void sm_init(sm *m,int l) {
	if(m->l)
		free(m->m);
	m->m=calloc(l,sizeof(char));
	dieif(!(m->m),"malloc failed (init stretchy mem)");
	m->l=l;
	m->i=0;
}
void sm_stretch(sm *m, int ld) {
	dieif(ld<1,"dont shrink the stretchy mem");
	m->m=realloc(m->m,(sizeof(char)*(m->l+ld)));
	m->l=m->l+ld;
}
void sm_append(sm *m,char *d, int l) {
	dieif(l<1,"cannot append negative length string");
	if(m->i+l>m->l)
		sm_stretch(m,(m->i+l)-m->l);
	memcpy(m->m+m->i,d,l);
	m->i+=l;
}
void sm_appendstr(sm *m, char *s) {
	sm_append(m,s,strlen(s));
}
void sm_free(sm *m) {
	free(m->m);
	free(m);
}
char *sm_extract(sm *m){
	char *s=calloc(m->i,sizeof(char));
	return s;
}
char *sm_dump(sm *m) {
	char *s=sm_extract(m);
	sm_free(m);
	return s;
}
sm *sm_new(int l){
	sm *m=calloc(sizeof(sm),sizeof(char));
	dieif(!m,"malloc failed (new stretchy mem)");
	sm_init(m,l);
	return m;
}

sm *sm_slice(sm *s,int a, int b) {
	dieif(a>=b,"You're an idiot stephen.");
	sm *d=sm_new(b-a);
	sm_append(d,s->m+a,b-a);
	return d;
}

void dieif(int c,char *m) {
	if(c){
		puts(m);
		if(dbg_context)
			puts(dbg_context);
		exit(c);
	}
}
int getavatar(void){
	puts("download avatar....");
	CURL *curl;
	curl=curl_easy_init();
	dieif(!curl,"curl init failed");
	FILE *jpgfp=fopen(PIC_FNAME,"wb");
	curl_easy_setopt(curl,CURLOPT_URL,PIC_URL);
	curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,fwrite);
	curl_easy_setopt(curl,CURLOPT_WRITEDATA,jpgfp);
	CURLcode res=curl_easy_perform(curl);
	printf("got %d\n",res);
	fclose(jpgfp);
	return 0;
}
void append_tag(sm *doc,char *t, char *attr,char *inner) {
	//TODO: probably pretty sluggish because of many calls to
	//sm_appendstr with one charecter strings
	sm_appendstr(doc,"<");
	sm_appendstr(doc,t);
	if(attr){
		sm_appendstr(doc," ");
		sm_appendstr(doc,attr);
	}
	sm_appendstr(doc,">");
	sm_appendstr(doc,inner);
	sm_appendstr(doc,"<\\");
	sm_appendstr(doc,t);
	sm_appendstr(doc,">");

}
void append_tagc(sm *doc, char *t, char *atter, char *inner){
	append_tag(doc,t,atter,inner);
	free(inner);
}
char *mkindex(){
	sm *doc=sm_new(2);
	sm *body=sm_new(2);
	sm *header=sm_new(2);
	append_tag(doc,"html",NULL,"hello world");
	sm_free(body);
	sm_free(header);
	return sm_dump(doc);
}
void save(char *text, char *name) {
	FILE *fp=fopen(name,"w");
	puts(name);
	dieif(!fp,"could not open file for writting");
	dieif(fwrite(text,sizeof(char),strlen(text),fp)!=strlen(text),"incomplete write");
	fclose(fp);

}

int main() {
	//dieif(getavatar(),"could not get avatar");
	char *page=mkindex();
	dieif(!page,"whoops (index page string pointer)");
	save(page,"index.html");
	puts("cleanup from index.html....");
	free(page);

}
