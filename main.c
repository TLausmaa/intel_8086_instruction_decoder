#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define INST_MASK 0xFC
#define MOV_D_MASK 0x02
#define MOV_W_MASK 0x01
#define MOV_ENC 0x88
#define REG_MASK 0x38 // 00111000
#define RM_MASK  0x07 // 00000111
#define MOV "mov"

// W-bit used to index these
const char* mem_to_mem_REG_and_RM_encodings[2][8] = 
{
    {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

typedef struct Binary
{
    uint8_t* data;
    unsigned int byte_count;
} Binary;

Binary read_binary_instructions(const char* filename)
{
	FILE* f = fopen(filename, "rb");
	
	if (f == NULL) 
	{
		printf("Reading binary instructions failed\n");
		exit(1);
	}

	fseek(f, 0, SEEK_END);
	long fsize = ftell(f);
	fseek(f, 0, SEEK_SET);

	uint8_t* binary = malloc(sizeof(uint8_t) * fsize);
	fread(binary, fsize, 1, f);
	fclose(f);

	Binary container;
	container.data = binary;
	container.byte_count = fsize;
	return container;
}

void print_bin(uint8_t value)
{
    for (int i = sizeof(char) * 7; i >= 0; i--) 
    {
        printf("%d", (value & (1 << i)) >> i);
    }
    putc('\n', stdout);
}

void print_assembly(int is_16_bit, const char* instruction, const char* operand1, const char* operand2)
{
    if (is_16_bit)
    {
        printf("bits 16\n\n");
    }
    else
    {
        printf("bits 8\n\n");
    }

    printf("%s %s, %s\n", instruction, operand1, operand2);
}

void lookup_instruction(uint8_t inst, uint8_t byte1, uint8_t byte2) 
{
    switch (inst) 
    {
        case MOV_ENC: 
        {
            uint8_t mov_d_field = byte1 & MOV_D_MASK;
            uint8_t mov_w_field = byte1 & MOV_W_MASK;
            uint8_t reg = byte2 & REG_MASK;
            uint8_t rm = byte2 & RM_MASK;
            int is_destination_in_reg = mov_d_field == MOV_D_MASK;
            int operates_on_word_data = mov_w_field == MOV_W_MASK;

            const char* register1 = mem_to_mem_REG_and_RM_encodings[mov_w_field][reg >> 3];
            const char* register2 = mem_to_mem_REG_and_RM_encodings[mov_w_field][rm];
            const char* source = is_destination_in_reg ? register2 : register1;
            const char* destination = is_destination_in_reg ? register1 : register2;
            print_assembly(operates_on_word_data, MOV, destination, source);
        }
            break;
        default: 
            printf("Instruction not detected\n");
            break;
    }
}

int main() 
{
    Binary container = read_binary_instructions("listing_37_mov");
    uint8_t encoded_instruction = (container.data[0] & INST_MASK);
    lookup_instruction(encoded_instruction, container.data[0], container.data[1]);
    return 0;
}