# shellcode-test
##整个实验分成了3步骤:
stack flow hacker
64位系统
压栈顺序基本为：参数从右到左（不过64位系统参数用寄存器保存），下条指令地址作为返回地址，原调用栈rbp
接着就是被调用函数的rbp位置，rsp-0xxxx给被调用函数栈利用，包括被调用函数局部变量。
原理：
要是能够利用栈溢出bug覆盖栈里的返回地址，变成我自己想要执行的code地址。
如test.c里的hacker函数；
知识点：
进入func函数，反汇编代码：
   0x00000000004005dd <+0>:	push   %rbp
   0x00000000004005de <+1>:	mov    %rsp,%rbp
   0x00000000004005e1 <+4>:	sub    $0x20,%rsp
   0x00000000004005e5 <+8>:	mov    %rdi,-0x18(%rbp)
=> 0x00000000004005e9 <+12>:	movl   $0x0,-0x10(%rbp)
   0x00000000004005f0 <+19>:	mov    -0x18(%rbp),%rcx
 寄存器：
 (gdb) x/10x $rbp
0x7fffffffe500:	0xffffe550	0x00007fff	0x004006b2	0x00000000
0x7fffffffe510:	0x31313131	0x31313131	0x32323232	0x33333333
0x7fffffffe520:	0x34343434	0x35353535
局部变量(gdb) p	&str
$2 = (char (*)[4]) 0x7fffffffe4f0
可见从str地址0x7ffffffe4f0到调用函数下条地址即返回地址0x7ffffffe508总共ox18，也就是24（换台机器可能就不是这个值），所以test.c确定pEIP地址的时候是数组的第24位。
再就是如何确定拷贝多少字节，
查看shellcode的内存情况：
(gdb) p	shellcode
$4 = 0x7fffffffe510 "111111112222333344445555\033\006@"
(gdb) x/10x 0x7fffffffe510
0x7fffffffe510:	0x31313131	0x31313131	0x32323232	0x33333333
0x7fffffffe520:	0x34343434	0x35353535	0x0040061b	0x00000000
0x7fffffffe530:	0x38383838	0x39393939
所以现在是将地址0x7fffffffe510内容复制到0x7fffffffe4f0，如果复制32字节，恰好将从0x7fffffffe4f0到返回地址的内容全部覆盖掉，不会出现覆盖重叠，
可以得到预期的结果，顺利的调到hacker函数。
