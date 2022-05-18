
#include <iostream>

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


uint8_t search_high_bit(uint16_t val) {
    uint8_t i = 0;
    while (val) {
        ++i;
        val >>= 1;
    }
    return i;
}

uint8_t gdiv(uint16_t a,uint16_t b,uint16_t* remainder) {
    uint16_t r0 = 0;
    uint8_t qn = 0;
    int cnt = 0;
    r0 = a;

    uint8_t x1 = search_high_bit(r0);
    uint8_t x2 = search_high_bit(b);
    cnt = x1 - x2;

    while (cnt >= 0) {
        // 商的表示形式
        qn |= (1 << cnt);
        // 加或者减就是XOR
        r0 ^= (b << cnt);
        // 重新计算可用的商
        x1 = search_high_bit(r0);
        cnt = x1 - x2;
    }

    *remainder = r0;
    return qn;
}

uint8_t gmult_inverse(uint16_t a,uint16_t b) {
    uint16_t r0 = 0,r1 = 0,r2 = 0;
    uint8_t qn = 0,v0 = 1,w0 = 0,v1 = 0,w1 = 1;
    uint8_t v2 = 0,w2 = 0;

    r0 = b;
    r1 = a;

    while (r1 != 1) {
        qn = gdiv(r0, r1, &r2);

        v2 = v0 ^ (gmult(qn,v1));
        w2 = w0 ^ (gmult(qn,w1));

        r0 = r1;
        r1 = r2;

        v0 = v1;
        v1 = v2;

        w0 = w1;
        w1 = w2;
    }

    return w1;
}

//S盒字节变换
uint8_t byteTransformation(uint8_t a, uint8_t c) {
    uint8_t tmp[8]={0};
    for(uint8_t i=0;i<8;i++) {
        tmp[i]= (((a>>i)&0x1)^((a>>((i+4)%8))&0x1)^((a>>((i+5)%8))&0x1)^((a>>((i+6)%8))&0x1)^((a>>((i+7)%8))&0x1)^((c>>i)&0x1)) << i;
    }
    tmp[0] = tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]+tmp[6]+tmp[7];
    return tmp[0];
}

//逆S盒字节变换
uint8_t invByteTransformation(uint8_t a, uint8_t d) {
    uint8_t tmp[8]={0};
    for(uint8_t i=0;i<8;i++) {
        tmp[i]= (((a>>((i+2)%8))&0x1)^((a>>((i+5)%8))&0x1)^((a>>((i+7)%8))&0x1)^((d>>i)&0x1)) << i;
    }
    tmp[0] = tmp[0]+tmp[1]+tmp[2]+tmp[3]+tmp[4]+tmp[5]+tmp[6]+tmp[7];
    return tmp[0];
}

void print_box(uint8_t s_box_ary[16][16]) {
    printf("    0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F");
    for(int i=0;i<0x10;i++) {
        printf("\r\n%2x",i);
        for(int j=0;j<0x10;j++) {
            printf(" %2x",s_box_ary[i][j]);
        }
    }
    printf("\r\n\n");
}

//S盒产生
void s_box_gen(void) {
    uint8_t i,j;
    uint8_t s_box_ary[16][16] = {0};
    
    //初始化S盒
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            s_box_ary[i][j] = ((i<<4)&0xF0) + (j&(0xF));
        }
    }
    print_box(s_box_ary);

    //求在GF(2^8)域上的逆，0映射到自身
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            if(s_box_ary[i][j] != 0) {
                s_box_ary[i][j] = gmult_inverse(s_box_ary[i][j],0x11B);
            }
        }
    }
    print_box(s_box_ary);

    //对每个字节做变换
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            s_box_ary[i][j] = byteTransformation(s_box_ary[i][j], 0x63);
        }
    }
    print_box(s_box_ary);
}

//逆S盒产生
void inv_s_box_gen(void)
{
    uint8_t i,j;
    uint8_t s_box_ary[16][16] = {0};
    uint8_t b=0, bb=0;

    //初始化S盒
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            s_box_ary[i][j] = ((i<<4)&0xF0) + (j&(0xF));
        }
    }
    print_box(s_box_ary);

    //对每个字节做变换
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            s_box_ary[i][j]=invByteTransformation(s_box_ary[i][j], 0x05);
        }
    }
    print_box(s_box_ary);

    //求在GF(2^8)域上的逆，0映射到自身
    for(i=0;i<0x10;i++) {
        for(j=0;j<0x10;j++) {
            if(s_box_ary[i][j] != 0) {
                s_box_ary[i][j] = gmult_inverse(s_box_ary[i][j],0x11B);
            }
        }
    }
    print_box(s_box_ary);
}

int main() {
    /// 0000 0000 1000 0011
    // uint8_t a = 0x83; // x^7 + x + 1
    // 0000 0001 0001 1011
    // uint16_t b = 0x11b; // x^8 + x^4 + x^3 + x + 1
    // int cnt = search_high_bit(b);
    // printf("cnt :%d\n",cnt);
    // uint8_t re = 0;
    // uint8_t R = gdiv(a,0x1b,&re);
    // // 0000 1110
    // printf("gdiv : %x,remainder:%x\n",R,re);// x^3 + x^2 +x

    // // 计算乘法逆元
    // uint8_t R = gmult_inverse(a,b);
    // printf("gmult_inverse :%x\n",R);

    // s_box_gen();
    inv_s_box_gen();
    return 0;
}