//Stephen Marty

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86emul.h"
#define limit 2000

int registers[9];
int pc;
int ZF, OF, SF;
int debug = 0;
char *memory;

void tracker(char*msg) {
	if (debug==1) {
		if (msg[0] != '\0') {
			printf("Instruction: %s; ", msg);
		}
		printf("pc value: %d\n", pc);
		//printf("Line number %d in file %s\n", __LINE__, __FILE__);
	}
}

void push(int regindex) {
	int regval = registers[regindex];
	int regval2 = registers[4]-4;
	int *memArray = (int*)(memory+regval2);
	*memArray = regval;
	registers[4] = regval2;
	pc += 2;
}

void pop(int regindex) {
	int regval = registers[4];
	int regval2 = regval+4;
	int *memArray = (int*)(memory+regval);
	registers[4] = regval2;
	registers[regindex] = *memArray;
	pc += 2;
}

void decode(unsigned char *byte) {
	bfield *newbfield = (bfield*)(memory+pc+1);
	int reghi = newbfield->hi;
	int reglo = newbfield->lo;
	
	int reghival = registers[reghi];
	int regloval = registers[reglo];

	int sum = reghival + regloval;
	int diff = reghival - regloval;
	int and = reghival & regloval;
	int xor = reghival ^ regloval;
	int product = reghival * regloval;
	
	unsigned char inputChar;
	int inputNum;
	
	switch (*byte) {
	case 0x00: //nop
	{
		pc += 1;
		tracker("nop");
		break;
	}
	case 0x10: //halt
		curr_state = HLT;
		break;
	case 0x20: //rmovl
	{
		registers[newbfield->hi] = registers[newbfield->lo];
		pc += 2;
		tracker("rrmovl");
		break;
	}
	case 0x30: //irmovl
	{
		int *num = (int*)(memory+pc+2);
		registers[newbfield->hi] = *num;
		pc += 6;
		tracker("irmovl");
		break;
	}
	case 0x40: //rmmovl
	{
		int *num = (int*)(memory+pc+2);
		int address = registers[newbfield->hi]+*num;
		int *memArray = (int*)(memory+address);
		*memArray = registers[newbfield->lo];
		pc += 6;
		tracker("rmmovl");
		break;
	}
	case 0x50: //mrmovl
	{
		int *num = (int*)(memory+pc+2);
		int address = registers[newbfield->hi]+*num;
		int *memArrayAddress = (int*)(memory+address);
		registers[newbfield->lo] = *memArrayAddress;
		pc += 6;
		tracker("mrmovl");
		break;
	}
	case 0x60: //addl
		registers[reghi] = sum;
		if (sum == 0) ZF = 1;
		else ZF = 0;
		
		if ((regloval > 0 && reghival > 0 && sum < 0) || (regloval < 0 && reghival < 0 && sum > 0)) OF = 1;
		else OF = 0;
		
		if (sum < 0) SF = 1;
		else SF = 0;
		
		pc += 2;
		break;
	case 0x61: //subl
		if ((reghi >= 0 && reghi <= 8) && (reglo >= 0 && reglo <= 8)) {
			registers[reghi] = diff;
			if (diff == 0) ZF = 1;
			else ZF = 0;

			if ((regloval < 0 && reghival > 0 && diff < 0) || (regloval > 0 && reghival < 0 && diff > 0)) OF = 1;
			else OF = 0;

			if (diff < 0) SF = 1;
			else SF = 0;
			pc += 2;
		}
		else {
			curr_state = ERR;
		}
		break;
	case 0x62: //andl
		if ((reghi >= 0 && reghi <= 8) && (reglo >= 0 && reglo <= 8)) {
			registers[reghi] = and;
			if (and == 0) ZF = 1;
			else ZF = 0;

			if (and < 0) SF = 1;
			else SF = 0;

			OF = 0;
			pc += 2;
		}
		else {
			curr_state = ERR;
		}
		break;
	case 0x63: //xorl
		if ((reghi >= 0 && reghi <= 8) && (reglo >= 0 && reglo <= 8)) {
			registers[reghi] = xor;
			if (xor == 0) ZF = 1;
			else ZF = 0;
			
			if (xor < 0) SF = 1;
			else SF = 0;

			OF = 0;
			pc += 2;
		}
		else {
			curr_state = ERR;
		}
		break;
	case 0x64: //mull
		if ((reghi >= 0 && reghi <= 8) && (reglo >= 0 && reglo <= 8)) {
			registers[reghi] = product;
			if (product == 0) ZF = 1;
			else ZF = 0;

			if ((regloval > 0 && reghival > 0 && product < 0) || 
			(regloval < 0 && reghival < 0 && product < 0) || 
			(regloval < 0 && reghival > 0 && product > 0) || 
			(regloval > 0 && reghival < 0 && product > 0)) OF = 1;
			else OF = 0;

			if (product < 0) SF = 1;
			else SF = 0;

			pc += 2;
		}
		else {
			curr_state = ERR;
		}
		break;
	case 0x70: //jmp
	{
		int *num = (int*)(memory+pc+1);
		pc = *num;
		break;
	}
	case 0x71: //jle
	{
		int *num = (int*)(memory+pc+1);
		if (((SF ^ OF) | ZF) == 1) pc = *num;
		else pc += 5;
		break;
	}
	case 0x72: //jl
	{
		int *num = (int*)(memory+pc+1);
		if ((SF ^ OF) == 1) pc = *num;
		else pc += 5;
		break;
	}
	case 0x73: //je
	{
		int *num = (int*)(memory+pc+1);
		if (ZF == 1) pc = *num;
		else pc += 5;
		break;
	}
	case 0x74: //jne
	{
		int *num = (int*)(memory+pc+1);
		if (ZF == 0) pc = *num;
		else pc += 5;
		break;
	}
	case 0x75: //jge
	{
		int *num = (int*)(memory+pc+1);
		if ((SF ^ OF) == 0) pc = *num;
		else pc += 5;
		break;
	}
	case 0x76: //jg
	{
		int *num = (int*)(memory+pc+1);
		if (((SF ^ OF) & ZF) == 0) pc = *num;
		else pc += 5;
		break;
	}
	case 0x80: //call
	{
		int *num = (int*)(memory+pc+1);
		registers[8] = pc+5;
		push(8);
		pc = *num;
		break;
	}
	case 0x90: //ret
	{
		pop(8);
		pc = registers[8];
		break;
	}
	case 0xa0: //pushl
	{
		bfield *newbfield = (bfield*)(memory+pc+1);
		int regindex = newbfield->lo;
		if ((newbfield->hi == 0xf) && (regindex >= 0 && regindex <= 8)) {
			push(regindex);
		}
		else {
			curr_state = ERR;
		}
		break;
	}
	case 0xb0: //popl
	{
		bfield *newbfield = (bfield*)(memory+pc+1);
		int regindex = newbfield->lo;
		if ((newbfield->hi == 0xf) && (regindex >= 0 && regindex <= 8)) {
			pop(regindex);
		}
		else {
			curr_state = ERR;
		}
		break;
	}
	case 0xc0: //readb
	{
		int *num = (int*)(memory+pc+2);
		OF = 0;
		SF = 0;
		if (scanf("%c", &inputChar) == EOF) {
			ZF = 1;
		}
		else {
			unsigned char *byteToRead = (unsigned char*)(memory+registers[reglo]+*num);
			ZF = 0;
			*byteToRead = inputChar;
		}
		pc += 6;
		break;
	}
	case 0xc1: //readl
	{
		int *num = (int*)(memory+pc+2);
		OF = 0;
		SF = 0;
		if (scanf("%d", &inputNum) == EOF) {
			ZF = 1;
		}
		else {
			int *longread = (int*)(memory+registers[reglo]+*num);
			ZF = 0;
			*longread = inputNum;
		}
		pc += 6;
		break;
	}
	case 0xd0: //writeb
	{
		int *num = (int*)(memory+pc+2);
		unsigned char *towrite = (unsigned char*)(memory+registers[reglo]+*num);
		printf("%c", *towrite);
		pc += 6;
		break;
	}
	case 0xd1: //writel
	{
		int *num = (int*)(memory+pc+2);
		int *longwrite = (int*)(memory+registers[reglo]+*num);
		printf("%d", *longwrite);
		pc += 6;
		break;
	}
	default:
		curr_state = ERR;
		break;
	}
}

void fetch(char* memory) {
	while (curr_state == AOK) {
		unsigned char *curr_byte;
		curr_byte = (unsigned char*)(memory+pc);
		decode(curr_byte);
	}
	
	if (curr_state == HLT) {
		exit(0);
	}
	
	else if (curr_state == ERR) {
		fprintf(stderr, "ERROR: Invalid instruction.");
		exit(1);
	}
}

void txtmem(char *token, int i) {
	int strpass = 0;
	int pass = 16;
	while (strpass < strlen(token)) {
		bfield *newbfield = (bfield*)memory+i;
		char *upper = (char*)malloc(1);
		strncpy(upper, token+strpass+1, 1);
		newbfield->hi = strtol(upper, NULL, pass);
		char *lower = (char*)malloc(1);
		strncpy(lower, token+strpass, 1);
		newbfield->lo = strtol(lower, NULL, pass);
		strpass += 2;
		i++;
	}
}
void strmem(char *str, int i) {
	while(*str != '\0') {
		memory[i] = *str;
		str++;
		i++;
	}
	//tracker("");
}

int main(int argc, char ** argv) {
	if (argc == 1 || argc > 3) {
		fprintf(stderr, "ERROR: Invalid arguments.\n");
		exit(1);
	}
	
	else if (strcmp(argv[1], "-h") == 0) {
		printf("Usage: ./y86emul [-h] <y86 input file>\n");
		exit(0);
	}
	else {
		FILE *file = fopen(argv[1], "r");
		char line[limit];
		char str[6];
		char *token;
		char *breaks = " \n\t";
		int lineiter = 0;
		int size;
		int i;
		int scount = 0;
		
		while (fgets(line, limit, file) != NULL) {
			token = strtok(line, breaks);
			if (strcmp(token, ".size") == 0) {
				if (scount == 0) {
				sscanf(line, "%s %x\n", str, &size);
				memory = (char*)malloc(sizeof(char)*size);
				scount++;
				}
				else {
					fprintf(stderr, "ERROR: Y86 file contains multiple .size specifications.\n");
				}
			}
			//.bss not needed
			else if (strcmp(token, ".text") == 0) {
				token = strtok(NULL, breaks);
				sscanf(token, "%x", &i);
				pc = i;
				token = strtok(NULL, breaks);
				txtmem(token, i);

			}
			
			else if (strcmp(token, ".byte") == 0) {
				token = strtok(NULL, breaks);
				sscanf(token, "%x", &i);
				token = strtok(NULL, breaks);
				int nump;
				char bchar;
				sscanf(token, "%2x", &nump);
				bchar = nump;
				memory[i] = bchar;
			}
			
			else if (strcmp(token, ".string") == 0) {
				token = strtok(NULL, breaks);
				sscanf(token, "%x", &i);
				token = strtok(NULL, breaks);
				token++;
				while (token != NULL) {
					char *token2 = strtok(NULL, breaks);
					int len;
					
					if (token2 != NULL) {
						len = strlen(token);
						strmem(token, i);
						i += len;
						memory[i] = ' ';
						i++;
						token = token2;
					}
					
					else {
						int lastChar = strlen(token)-1;
						token[lastChar] = '\0';

						strmem(token, i);
						i += strlen(token);
						i++;
						memory[i] = '\0';
						break;
					}
				}
			}
			
			else if (strcmp(token, ".long") == 0) {
				token = strtok(NULL, breaks);
				sscanf(token, "%x", &i);
				token = strtok(NULL, breaks);
				int num;
				sscanf(token, "%d", &num);
				memory[i] = num;
			}
			
			else {
				fprintf(stderr, "ERROR: Invalid instruction.\n");
				exit(1);
			}
			lineiter++;
		}
		fclose(file);
		curr_state = AOK;
		fetch(memory);
	}
	return 0;
}

