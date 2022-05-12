#include<stdio.h>

char dict[]={0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
char DisplayCache[9] = { 0x3F, 0x3f, 0x3f, 0x3f, 0x3f, 0x3F, 0x3f, 0x3f, 0x3f};

char tmp;
int tmp2,i,n;
int CurSeconds=0; 


int main(){
	int2Cache(2);
	return 0;
}


int int2Cache(CurSeconds)
{
	tmp = CurSeconds /1000000;
        DisplayCache[2] = dict[tmp];
        tmp = CurSeconds %1000000/100000;
        DisplayCache[3] = dict[tmp];
        tmp = CurSeconds %100000/10000;
        DisplayCache[4] = dict[tmp];
        tmp = CurSeconds %10000/1000;
        DisplayCache[5] = dict[tmp];
        tmp = CurSeconds %1000/100;
        DisplayCache[6] = dict[tmp];
        tmp = CurSeconds %100/10;
        DisplayCache[7] = dict[tmp];
        tmp = CurSeconds %10;
        DisplayCache[8] = dict[tmp];
       
       for(i=0;i<=8;i++)
       {
       printf("%x ",DisplayCache[i]);
       	}    
   }

