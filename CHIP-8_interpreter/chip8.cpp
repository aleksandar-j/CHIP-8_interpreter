
#include "chip8.hpp"

Chip8::Chip8(const uint16_t* instructions, uint32_t* pixels, int width, int height)
{

    memcpy(this->M + 0x200, instructions, 4096 - 0x200);

    PC = 0x200;

    PIXELS = pixels;
    this->width = width;
    this->height = height;
    wh_multiplier = width / 64;

}

#define WHITE (0xFFFFFFFF)
#define BLACK (0x00000000)

#define X ((uint8_t)((CINSTR & 0x0F00) >> 8))
#define VX (V[X])
#define Y ((uint8_t)((CINSTR & 0x00F0) >> 4))
#define VY (V[Y])

#define VF (V[0xF])

#define NN (CINSTR & 0x00FF)
#define NNN (CINSTR & 0x0FFF)

void Chip8::execute_cycle()
{

    const uint16_t CINSTR = (M[PC] << 8) + M[PC + 1];
    inc_PC();

    switch ((CINSTR & 0xF000) >> 12) 
    {
        
        // Seems clean
        case 0:
        {
            switch (NN)
            {
                case 0xE0: 
                {
                    // Clears the screen.
                    for (size_t x = 0; x < 64; x++) {
                        for (size_t y = 0; y < 32; y++) {
                            Chip8::color_pixel_real(x, y, BLACK);
                        }
                    }
                } break;

                case 0xEE:
                {
                    // Returns from a subroutine.
                    PC = S.top();
                    S.pop();
                } break;
            }
        } break;
        
        // Seems clean
        case 1:
        {
            // Jumps to address NNN.
            PC = NNN;
        } break;
        
        // Seems clean
        case 2:
        {
            // 	Calls subroutine at NNN.
            S.push(PC);
            PC = NNN;
        } break;

        // Seems clean
        case 3:
        {
            // Skips the next instruction if VX equals NN. (Usually the next instruction is a jump to skip a code block)
            if (VX == NN) {
                inc_PC();
            }
        } break;

        // Seems clean
        case 4:
        {
            // Skips the next instruction if VX doesn't equal NN. (Usually the next instruction is a jump to skip a code block)
            if (VX != NN) {
                inc_PC();
            }
        } break;

        // Seems clean
        case 5:
        {
            // Skips the next instruction if VX equals VY. (Usually the next instruction is a jump to skip a code block)
            if (VX == VY) {
                inc_PC();
            }
        } break;

        // Seems clean
        case 6:
        {
            // Sets VX to NN.
            VX = NN;
        } break;

        // Seems clean
        case 7:
        {
            // 	Adds NN to VX. (Carry flag is not changed)
            VX += NN;
        } break;

        case 8:
        {
            switch (CINSTR & 0x000F) 
            {
                // Seems clean
                case 0: 
                {
                    // 	Sets VX to the value of VY.
                    VX = VY;
                } break;

                // Seems clean
                case 1:
                {
                    // Sets VX to VX or VY. (Bitwise OR operation)
                    VX |= VY;
                } break;

                // Seems clean
                case 2:
                {
                    // Sets VX to VX and VY. (Bitwise AND operation)
                    VX &= VY;
                } break;

                // Seems clean
                case 3:
                {
                    // Sets VX to VX xor VY.
                    VX ^= VY;
                } break;

                // Seems clean
                case 4:
                {
                    // Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
                    uint32_t result = ((uint32_t)VX) + ((uint32_t)VY);
                    
                    if (result > 0xFF) {
                        VF = 1;
                    } else {
                        VF = 0;
                    }

                    VX = result & 0xFF;
                } break;

                // Seems clean
                case 5:
                {
                    // Let VX = VX - VY (VF = 00 if VX < VY, VF = 01 if VX g.e. VY)
                    if (VX < VY) {
                        VF = 0;
                    } else {
                        VF = 1;
                    }

                    VX -= VY;
                } break;

                // Seems not clean!!
                case 6: 
                {
                    // Shifts VY right by one and stores the result to VX (VY remains unchanged). VF is set to the value of the least significant bit of VY before the shift.
                    VF = VX & 1;
                    VX >>= 1;
                } break;

                default:
                {
                    printf("Stop!");
                } break;

            }
        } break;

        // Seems clean
        case 9:
        {
            // Skips the next instruction if VX doesn't equal VY. (Usually the next instruction is a jump to skip a code block)
            if (VX != VY) {
                inc_PC();
            }
        } break;

        // Seems clean
        case 0xA:
        {
            // Sets I to the address NNN.
            I = NNN;
        } break;

        // Seems clean
        case 0xB:
        {
            // Jumps to the address NNN plus V0.
            PC = V[0] + NNN;
        } break;

        // Seems clean
        case 0xC:
        {
            // Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
            VX = (rand() % (0xFF + 1)) & NN;
        } break;

        // Seems clean
        case 0xD:
        {
            // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value doesn’t change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen
            uint16_t draw_x = VX;
            uint16_t draw_y = VY;
            size_t bytes_to_read = CINSTR & 0x000F;

            for (size_t y = 0; y < bytes_to_read; y++) {
                
                uint8_t curr_byte = M[I + y];

                for (size_t x = 0; x < 8; x++) {
                    if ((curr_byte >> (7 - x)) & 1) {
                        // There is color here
                        Chip8::color_pixel(draw_x + x, draw_y + y, WHITE);
                    } else {
                        // No color
                        Chip8::color_pixel(draw_x + x, draw_y + y, BLACK);
                    }
                }

            }

            redraw_screen = true;
        } break;

        // Seems clean
        case 0xE: 
        {
            switch (NN)
            {

                // Seems clean
                case 0x9E:
                {
                    // Skips the next instruction if the key stored in VX is pressed. (Usually the next instruction is a jump to skip a code block)
                    if (pressed[VX]) {
                        inc_PC();
                    }
                } break;

                // Seems clean
                case 0xA1:
                {
                    // Skips the next instruction if the key stored in VX isn't pressed. (Usually the next instruction is a jump to skip a code block)
                    if (!pressed[VX]) {
                        inc_PC();
                    }
                } break;

            }
        } break;

        case 0xF:
        {
            switch (NN) 
            {

                // Seems clean
                case 0x07: 
                {
                    // Sets VX to the value of the delay timer.
                    // NOTE: App most likely sleeps here...
                    //Sleep(DT);
                    VX = DT;
                } break;

                // Seems clean
                case 0x0A: 
                {
                    // A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
                    VX = 0xFF;
                    for (uint8_t i = 0; i <= 0xF; i++) {
                        if (pressed[i]) {
                            VX = i;
                        }
                    }
                    if (VX == 0xFF) {
                        dec_PC();
                    }
                } break;

                // Seems clean
                case 0x15: 
                {
                    // Sets the delay timer to VX.
                    DT = VX;
                } break;

                // Seems clean
                case 0x18:
                {
                    // Sets the sound timer to VX
                    ST = VX;
                } break;

                // Seems clean
                case 0x1E: 
                {
                    // Adds VX to I.
                    I += VX;
                } break;

                // Seems clean
                case 0x29: 
                {
                    // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
                    I = VX*5;
                } break;

                case 0x33: 
                {
                    // Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I, the middle digit at I plus 1, and the least significant digit at I plus 2. (In other words, take the decimal representation of VX, place the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.)
                    int val = VX;
                    
                    M[I] = val % 10;
                    val /= 10;
                    M[I + 1] = val % 10;
                    val /= 10;
                    M[I + 2] = val % 10;
                    val /= 10;

                    swap(M[I], M[I + 2]);
                } break;

                // Seems clean
                case 0x55:
                {
                    // Stores V0 to VX (including VX) in memory starting at address I. I is increased by 1 for each value written.
                    for (size_t i = 0; i <= X; i++) {
                        M[I + i] = V[i];
                    }
                    I += X + 1;
                } break;

                // Seems clean
                case 0x65:
                {
                    // Fills V0 to VX (including VX) with values from memory starting at address I. I is increased by 1 for each value written.
                    for (size_t i = 0; i <= X; i++) {
                        V[i] = M[I + i];
                    }
                    I += X + 1;
                } break;

                default:
                {
                    printf("Stop!");
                } break;

            }
        } break;

    }

}

void Chip8::color_pixel(uint32_t x, uint32_t y, uint32_t color)
{

    x *= wh_multiplier; y *= wh_multiplier;

    for (size_t curr_x = x; curr_x < (x + wh_multiplier); curr_x++) {
        for (size_t curr_y = y; curr_y < (y + wh_multiplier); curr_y++) {  
            uint32_t* curr_pixel = &(PIXELS[(curr_y % height)*width + (curr_x % width)]);

            if (*curr_pixel != ((*curr_pixel) ^ color)) {
                VF = true;
            }

            *curr_pixel ^= color;          
        }
    }

}

void Chip8::color_pixel_real(uint32_t x, uint32_t y, uint32_t color)
{

    x *= wh_multiplier; y *= wh_multiplier;

    for (size_t curr_x = x; curr_x < (x + wh_multiplier); curr_x++) {
        for (size_t curr_y = y; curr_y < (y + wh_multiplier); curr_y++) {
            uint32_t* curr_pixel = &(PIXELS[(curr_y % height)*width + (curr_x % width)]);

            *curr_pixel = color;
        }
    }

}

void Chip8::inc_PC()
{

    PC += 2;

}

void Chip8::dec_PC()
{

    PC -= 2;

}
