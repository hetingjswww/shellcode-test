#include <stdio.h>
#include <string.h>
int func(char *shellcode)
{
	char str[4] = "";
	memcpy(str,shellcode,32);
	printf("%s\n",str);
	return 1;
}

void hacker()  
{  
    printf("hello world");
} 
int main()
{
	char str[]="1111111122223333444455556666777788889999";
	int i = sizeof(long);
	long *pEIP = (long*)&str[24];
	*pEIP = &hacker;
	func(str);
	printf("return from func()\n");
	return 0;
}
