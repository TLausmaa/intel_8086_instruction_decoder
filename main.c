#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define MOV_REG_TO_REG_ENC 0x88
#define MOV_IMMEDIATE_TO_REG_ENC 0xB0
#define MOV_D_MASK 0x02
#define MOV_W_MASK 0x01
#define REG_MASK 0x38 // 00111000
#define RM_MASK  0x07 // 00000111
#define MOV "mov"

const int INSTRUCTION_MASKS_COUNT = 4;

const int instruction_masks[4] = 
{
    0xF0, 0xF8, 0xFC, 0xFE
};

// Indexed by: W, R/M 
const char* register_table[2][8] = 
{
    {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"},
    {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"}
};

// Indexed by: R/M
const char* effective_address_table[8] = 
{
    "bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"
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

void print_assembly_ss(const char* instruction, const char* operand1, const char* operand2)
{
    printf("%s %s, %s\n", instruction, operand1, operand2);
}

void print_assembly_si(const char* instruction, const char* operand1, const int operand2)
{
    printf("%s %s, %d\n", instruction, operand1, operand2);
}

void format_effective_address_calculation(char** result, const char* address, uint16_t displacement)
{
    *result = malloc(sizeof(char) * 64);
    if (displacement == 0)
    {
        snprintf(*result, 63, "[%s]", address);    
    }
    else
    {
        snprintf(*result, 63, "[%s + %d]", address, displacement);
    }
}

int decode_instruction(Binary* instructions, int read_position)
{
    uint8_t byte1 = instructions->data[read_position];

    for (int i = 0; i < INSTRUCTION_MASKS_COUNT; i++) 
    {
        uint8_t instruction = byte1 & instruction_masks[i];
        
        switch (instruction) 
        {
            case MOV_REG_TO_REG_ENC: 
            {
                uint8_t byte2 = instructions->data[read_position + 1];
                uint8_t mov_d_field = byte1 & MOV_D_MASK;
                uint8_t mov_w_field = byte1 & MOV_W_MASK;
                uint8_t mod_field = byte2 & 0xC0;
                uint8_t reg = byte2 & REG_MASK;
                uint8_t rm = byte2 & RM_MASK;
                char d_bit_on = mov_d_field == MOV_D_MASK;

                if (mod_field == 0xC0)
                {
                    const char* register1 = register_table[mov_w_field][reg >> 3];
                    const char* register2 = register_table[mov_w_field][rm];
                    const char* source = d_bit_on ? register2 : register1;
                    const char* destination = d_bit_on ? register1 : register2;
                    print_assembly_ss(MOV, destination, source);
                    return 1;
                }
                else 
                {
                    char* operand2 = NULL;
                    const char* register1 = register_table[mov_w_field][reg >> 3];
                    const char* address = effective_address_table[rm];
                    uint16_t displacement = 0;
                    
                    if (mod_field == 0x40) 
                    {
                        displacement = instructions->data[read_position + 2];
                    }
                    
                    if (mod_field == 0x80 || (mod_field == 0x00 && rm == 0x06)) 
                    {
                        displacement = (instructions->data[read_position + 3] << 8) + instructions->data[read_position + 2];
                    } 
                    
                    format_effective_address_calculation(&operand2, address, displacement);
                    const char* source = d_bit_on ? operand2 : register1;
                    const char* destination = d_bit_on ? register1 : operand2;
                    print_assembly_ss(MOV, destination, source);
                    free(operand2);
                    return 1 + (mod_field >> 6); // add number of displacement bytes
                }
            }
            case MOV_IMMEDIATE_TO_REG_ENC: 
            {
                uint8_t byte2 = instructions->data[read_position + 1];
                uint8_t w_field = (byte1 & 0x08) >> 3;
                uint8_t reg_field = byte1 & 0x07;
                char is_16_bit = w_field == 0x01;
                const char* reg = register_table[w_field][reg_field];
                if (is_16_bit)
                {
                    uint8_t byte3 = instructions->data[read_position + 2];
                    print_assembly_si(MOV, reg, (byte3 << 8) + byte2);
                    return 2;
                } 
                else 
                {
                    print_assembly_si(MOV, reg, byte2);
                    return 1;
                }
                break;
            }
            default:
            {
                if (i == INSTRUCTION_MASKS_COUNT - 1) 
                {
                    printf("Instruction not detected: ");
                    print_bin(byte1);
                    exit(1);
                }
                break;
            }
        }
    }

    return 0;
}

int main(int argc, char **argv) 
{
    if (argc < 2) 
    {
        printf("Provide binary file to decode as the first argument. Usage: ./decoder [filename]\n");
        exit(1);
    }

    Binary instructions = read_binary_instructions(argv[1]);
    
    printf("bits 16\n\n");

    for (int i = 0; i < instructions.byte_count; i += 1) 
    {
        i += decode_instruction(&instructions, i);
    }

    return 0;
}