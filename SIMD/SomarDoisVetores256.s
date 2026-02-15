	.file	"SomarDoisVetores256.cpp"
	.text
#APP
	.globl _ZSt21ios_base_library_initv
#NO_APP
	.section	.text.startup,"ax",@progbits
	.p2align 4
	.globl	main
	.type	main, @function
main:
.LFB8474:
	.cfi_startproc
	endbr64
	vmovapd	a(%rip), %ymm2
	vmulpd	b(%rip), %ymm2, %ymm0
	movl	$32, %eax
	leaq	c(%rip), %rsi
	leaq	b(%rip), %rcx
	leaq	a(%rip), %rdx
.L2:
	vmovapd	(%rcx,%rax), %ymm3
	vmulpd	(%rdx,%rax), %ymm3, %ymm1
	vmovapd	%ymm1, (%rsi,%rax)
	addq	$32, %rax
	cmpq	$4096, %rax
	jne	.L2
	vmovapd	%ymm0, c(%rip)
	xorl	%eax, %eax
	vzeroupper
	ret
	.cfi_endproc
.LFE8474:
	.size	main, .-main
	.globl	c
	.bss
	.align 32
	.type	c, @object
	.size	c, 32
c:
	.zero	32
	.globl	b
	.align 32
	.type	b, @object
	.size	b, 32
b:
	.zero	32
	.globl	a
	.align 32
	.type	a, @object
	.size	a, 32
a:
	.zero	32
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
