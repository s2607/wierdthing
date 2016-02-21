//Copyright FEB 2016 Stephen William Wiley WTFPL
//cc -g -lm -lcurl mkpage.c -o mkpage &&./mkpage
//cc -g -lm -lcurl mkpage.c -o mkpage &&gdb -se=./mkpage
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
	char * a=m->m;
	a=a+1;
	a=a-1;
	m->m=a;//write check 
	m->m=realloc(m->m,(sizeof(char)*(m->l+ld)));
	m->l=m->l+ld;
	if(m->i>m->l)
		m->i=m->l;
}
void sm_append(sm *m,char *d, int l) {
	dieif(l<0,"cannot append negative length string");
	if(m->i+l>m->l)
		sm_stretch(m,(m->i+l)-m->l);
	memcpy(m->m+m->i,d,l);
	m->i+=l;
}
void sm_appendstr(sm *m, char *s) {
	sm_append(m,s,strlen(s));
}
void sm_appendstrf(sm *m, char *s) {
	sm_appendstr(m,s);
	free(s);
}
void sm_free(sm *m) {
	free(m->m);
	free(m);
}
char *sm_extract(sm *m){
	char *s=calloc(m->i+1,sizeof(char));
	memcpy(s,m->m,m->i);
	return s;
}
char *sm_dump(sm *m) {
	char *s=sm_extract(m);
	sm_free(m);
	return s;
}
char *sm_dumpstr(sm *m) {//ensure zero at end
	int i=m->i;
	char *s=sm_dump(m);//m invalid now
	*(s+i+1)=0;
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
int isspan(char *e) {
	//span elements should (can? TODO: read RFC...) not contain other elments
	char *s[]={"h1","h2","h3","h4","h5","a","i","image","br","tr","p",""};
	int i;
	for(i=0;strlen(s[i]);i++);
	int l=i;
	for(i=0;i<l;i++){
		if(!strcmp(s[i],e))
			return i-l;
	}
	return 0;
}
void append_tag(sm *doc,char *t, char *attr,char *inner) {
	//TODO: probably pretty sluggish because of many calls to
	//sm_appendstr with one charecter strings
	//allong with isspan 
	if(!isspan(t))
		sm_appendstr(doc,"\n");
	sm_appendstr(doc,"<");
	sm_appendstr(doc,t);
	if(attr){
		sm_appendstr(doc," ");
		sm_appendstr(doc,attr);
	}
	sm_appendstr(doc,">");
	sm_appendstr(doc,inner);
	sm_appendstr(doc,"</");
	sm_appendstr(doc,t);
	sm_appendstr(doc,">");
	if(!isspan(t))
		sm_appendstr(doc,"\n");

}
char *tagl(char *t, char *atter, char *inner){
	sm *doc=sm_new(100);
	append_tag(doc,t,atter,inner);
	return sm_dumpstr(doc);
}
char *tag(char *t, char *atter, char *inner){
	char *s=tagl(t, atter, inner);
	free(inner);
	return s;
}
void addlist(sm *doc,char *t,char * *rows) {
	int i=0;
	sm *list=sm_new(30);
	for(i=0;*(rows+i);i++)
		append_tag(list,"li",NULL,*(rows+i));
	char *s=sm_dump(list);
	append_tag(doc,t,NULL,s);
	free(s);
}
char *anchor(char *a,char *p,char *t) {
	sm *href=sm_new(10);
	sm_appendstr(href,"href=\"");
	sm_appendstr(href,p);
	sm_appendstr(href,t);
	sm_appendstr(href,"\"");
	char *s=sm_dumpstr(href);
	char *r=tagl("a",s,a);
	free(s);
	return r;
}
char *image(char *p,char *t) {
	sm *href=sm_new(10);
	sm_appendstr(href,"src=\"");
	sm_appendstr(href,p);
	sm_appendstr(href,t);
	sm_appendstr(href,"\"");
	char *s=sm_dumpstr(href);
	char *r=tagl("image",s,"");
	free(s);
	return r;
}
char *mailto(char *a) {
	return anchor(a,"mailto:",a);
}
char *all_header() {
	static char *s=NULL;
	if(s)
		free(s);
	s=" ";
	return s;
}

char *index_body() {//Calling this in a loop will leak memory
	//we only call it once so it's not a big deal
	sm *doc=sm_new(2);
	sm_appendstr(doc,tag("center",NULL,tagl("h1",NULL,"Stephen Wiley")));
	char *list[]={"phreaker2600","Goochland Virginia",mailto("swwiley@gmail.com"),0};//How's that for scary initializer!
	addlist(doc,"ul",&list);
	free(list[2]);
	sm_appendstr(doc,image("","avatar.jpg"));
	sm_appendstr(doc,anchor("about me","","aboutme.html"));
	return sm_dumpstr(doc);
}
char *mkindex(){
	sm *doc=sm_new(2);
	sm *body=sm_new(2);
	sm *header=sm_new(2);
	append_tag(header,"header",NULL,all_header());
	//sm_appendstr(header,"\n");
	append_tag(body,"body","bgcolor=\"cyan\"",index_body());
	//sm_appendstr(body,"\n");
	//subsections built, concatonate them

	sm_appendstrf(header,sm_dumpstr(body));
	append_tag(doc,"html",NULL,sm_dumpstr(header));
	return sm_dump(doc);
}
void save(char *text, char *name) {
	FILE *fp=fopen(name,"w");
	puts(name);
	dieif(!fp,"could not open file for writting");
	dieif(fwrite(text,sizeof(char),strlen(text),fp)!=strlen(text),"incomplete write");
	fclose(fp);
	puts("written");

}

int main() {
	//dieif(getavatar(),"could not get avatar");
	char *page=mkindex();
	dieif(!page,"whoops (index page string pointer)");
	save(page,"index.html");
	puts("cleanup from index.html....");
	free(page);

}
