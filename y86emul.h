#ifndef Y86EMUL_H_
#define Y86EMUL_H_

typedef struct {
	unsigned char hi: 4;
	unsigned char lo: 4;
} bfield;

typedef enum {AOK, HLT, ADR, ERR} state;
state curr_state;
/*
typedef enum {
	nop=    0x00,
	halt=   0x10,
	rrmovl= 0x20,
	irmovl= 0x30,
	rmmovl= 0x40,
	mrmovl= 0x50,
	
	addl=   0x60,
	subl=   0x61,
	andl=   0x62,
	xorl=   0x63,
	mull=   0x64,
	
	jmp=    0x70,
	jle=    0x71,
	jl=     0x71,
	je=     0x72,
	jne=    0x74,
	jge=    0x75,
	jg=     0x75,
	
	pushl=  0xa0,
	popl=   0xb0,
	
	readb=  0xc0,
	readl=  0xc1,
	
	writeb= 0xd0,
	writel= 0xd1
}
*/

#endif /* Y86EMUL_H_ */
