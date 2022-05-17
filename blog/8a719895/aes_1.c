#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * Multiplication in GF(2^8)
 * http://en.wikipedia.org/wiki/Finite_field_arithmetic
 * Irreducible polynomial m(x) = x8 + x4 + x3 + x + 1
 */
uint8_t gmult(uint8_t a, uint8_t b) {

	uint8_t p = 0, i = 0, hbs = 0;

	for (i = 0; i < 8; i++) {
		if (b & 1) {
			p ^= a;
		}

		hbs = a & 0x80;
		a <<= 1;
		if (hbs) a ^= 0x1b; // 0000 0001 0001 1011	
		b >>= 1;
	}

	return (uint8_t)p;
}

/*
 * Generates the round constant Rcon[i]
 */
// uint8_t R[] = {0x02, 0x00, 0x00, 0x00};
uint8_t Rcon(uint8_t i) {
	uint8_t R = 0;
	if (i == 1) {
		R = 0x01; // x^(1-1) = x^0 = 1
	} else if (i > 1) {
		R = 0x02;
		i--;
		while (i-1 > 0) {
			R = gmult(R, 0x02);
			i--;
		}
	}
	
	return R;
}

int main() {
    uint8_t R[10] = {0};
    for (int i=0;i<10;i++) {
        R[i] = Rcon(i+1);
    }

    for (int i=0;i<10;i++) {
        printf((i+1)<10?"% 2d\t":"%d\t",i+1);
    }
    printf("\n");

    for (int i=0;i<10;i++) {
        printf("%02x\t",R[i]);
    } 
    printf("\n");
    return 0;
}

