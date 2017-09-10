# stack flow hacker
## 基础知识点:
64位系统
- 压栈顺序基本为：参数从右到左（不过64位系统参数用寄存器保存），下条指令地
址作为返回地址，原调用栈rbp
接着就是被调用函数的rbp位置，rsp-0xxxx给被调用函数栈利用，包括被调用函数局部变量。
- 原理：
要是能够利用栈溢出bug覆盖栈里的返回地址，变成我自己想要执行的code地址。
- 堆栈执行保护

OS | Execution space protection	 | ASLR (Address Space Layout Randomization) | Description | Check Tool
------------ | ------------- | ------------ | ------------ | ------------
Linux | Compiler (Link) Option:-Wl,-z,noexecstack or -Wa,--noexecstack System option: /proc/sys/kernel/exec-shield  | Compiler Option:-fPIC -pie System option:/proc/sys/kernel/randomize_va_space Or sysctl -w kernel.randomize_va_space=NEWVALUE | Linux has enabled a weak[6] form of ASLR by default since kernel version 2.6.12 (released June 2005).|1) http://www.trapkit.de/tools/checksec.html2) find /lib -exec execstack -q {} \; -print 2> /dev/null | grep ^X  to check whether the stack is executable 3) execstack -q ~/lib/libfoo.so.1 ~/bin/bar will query executable stack marking of the given files.4) Command readelf -h -d /usr/sbin/smbd | grep ‘Type:.*DYN’ If the file has been compiled for PIE, the command will return something similar to the following:Type: DYN (Shared object file)

## 整个实验分成了3步骤,由简入深:
- 修改返回地址eip值,执行函数hacker()
1. **test.c里的hacker函数**:
``` asm
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
```
2. **再就是如何确定拷贝多少字节**:
``` gdb
查看shellcode的内存情况：
(gdb) p	shellcode
$4 = 0x7fffffffe510 "111111112222333344445555\033\006@"
(gdb) x/10x 0x7fffffffe510
0x7fffffffe510:	0x31313131	0x31313131	0x32323232	0x33333333
0x7fffffffe520:	0x34343434	0x35353535	0x0040061b	0x00000000
0x7fffffffe530:	0x38383838	0x39393939
所以现在是将地址0x7fffffffe510内容复制到0x7fffffffe4f0，如果复制32字节，恰好将从0x7fffffffe4f0到返回地址的内容全部覆盖掉，不会出现覆盖重叠，
可以得到预期的结果，顺利的调到hacker函数。
```
- 返回地址指向一个栈地址
<>
如果返回地址被覆盖为一个栈地址,并且栈地址是我想执行的代码的机器码,那不就可以跟第一点的hacker函数一样达到目的了吗?错,你会遇到segmentfault,为了防止栈溢出攻击,由基础知识点中提到会取消栈的可执行权限属性,所以现在的目标是先让shellcode这个地址有可执行权限咯.
为了快速完成这个目的,就直接在main函数中显示的调用mprotect函数,改变shellcode地址属性,传入3个参数:需要改变属性地址(页对齐),大小,属性值,如果参数不多就会返回-1,并且errno是22,非法参数,成功返回0,但不调试,yeah,正确的新起了一个bash.表示有点兴奋.
</>
- shellcode包含调用mprotect的机器码
<>
这个才是需要动脑经的东东了,首先栈本身无可执行权限,并且mprotect的三个参数要传进去,即使前面两个都ok,从mprotect返回之后还要继续执行到shellcode对应起bash的code上去.
1. 栈本身无可执行权限,所以要让 shellcode的前8个字节(64位系统)就是执行真正的可执行代码地址
2. mprotect的参数可以由三个pop指令来完成,pop指令会将栈顶的值放到指定寄存器里,其实真实情况可能会待攻击的进程不太会有3个连续的pop指令在一起,可以去找3个pop ret指令去构造调用栈,这里为了简单起见,我假设被攻击进程里就有三个连续的pop指令在popfuc
3. mprotect的地址可以通过调试test.c编译出来的程序,得到mprotect地址.shellcode的地址也是通过单步调试得到.
既然1,2,3点里需要的东西都已经搞定了,那就向test.c文件里shellcode组成的注释代码,达到目的了.
</>


