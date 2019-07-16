/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
    /* use ~(~x & ~y) to represent x | y */
    int a = ~x & y;
    int b = x & ~y;
    return ~(~a & ~b);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    /* direct return tmin */
    return 1 << 31;
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
    /* use x + (x + 1) = 0xffffff,need exclude x = -1 situation */
    return !(~(x + x + 1)) & !!(x + 1);
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
    /* create mask for even number and use !(x ^ y) to represent equal */
    int mask = 0xaa;
    mask = mask | (mask << 8);
    mask = mask | (mask << 16);
    return !((x & mask) ^ mask);
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    /* use x + ~x + 1 = 0 */
    return ~x + 1;
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
    /* use ~x + 1 and sign bit,be careful about right shift */
    return (~((x + ~0x30 + 1) >> 31) & ((x + ~0x3a + 1) >> 31)) & 0x01;
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
    /* use !x and arithmetic right shift,mask is 0x00000000 or 0x11111111*/
    int mask = ((!x) << 31) >> 31;
    return (y & ~mask) + (z & mask);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    /* use sub may cause overflow */
    int diff = y + ~x + 1;
    int signx = (x >> 31) & 0x01;
    int signy = (y >> 31) & 0x01;
    //same sign and different sign
    return (~(diff >> 31) & 0x01 & !(signx ^ signy)) | (signx & !signy );
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
    /* sign bit of -0 and 0 are 0 */
    return ~((x | (~x + 1)) >> 31) & 0x01;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
    int sign = (x >> 31) & 0x01;
    int mask = (sign << 31) >> 31;
    int bit1,bit2,bit3,bit4,bit5;
    //flip  negetive number to find leftmost 0
    //negate is wrong
    x = ( ~x & mask) + (x & ~mask);
    //use right shift to find the leftmost 1 bit
    bit5 = !!(x >> 15);
    mask = (bit5 << 31) >> 31;
    x = ((x >> 16) & mask) + (x & ~mask);
    bit4 = !!(x >> 7);
    mask = (bit4 << 31) >> 31;
    x = ((x >> 8) & mask) + (x & ~mask);
    bit3 = !!(x >> 3);
    mask = (bit3 << 31) >> 31;
    x = ((x >> 4) & mask) + (x & ~mask);
    bit2 = !!(x >> 1);
    mask = (bit2 << 31) >> 31;
    x = ((x >> 2) & mask) + (x & ~mask);
    bit1 = !!(x);
    return (bit5 << 4) + (bit4 << 3) + (bit3 << 2) + (bit2 << 1) + bit1 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
    //special value
    if( ((uf >> 23) & 0xff) == 0xff )
        return uf;
    //denormalized value
    else if( ((uf >> 23) & 0xff) == 0x00 )
        return (uf & (1 << 31)) + (uf << 1 );
    //normalized value
    else
    {
        unsigned int value = uf + (1 << 23);
        //overflow
        if( ((value >> 23) & 0xff) == 0xff )
            return (value & (1 << 31)) + (0xff << 23);
        else
            return value;
    }
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
    /* use round down mode */
    int f = (1 << 23) + (uf & 0x7fffff);
    int exp = (uf >> 23) & 0xff;
    char sign = (uf >> 31) & 0x01;
    //special value,out of range
    if( exp == 0xff )
        return 0x80000000u;
    //denormalized value,round down to 0
    else if( exp == 0x00 )
        return 0;
    //normalized value
    else
    {
        exp -= 127;
        //when e >= 31,out of range
        if( exp >= 31 )
            return 0x80000000u;
        //right shift or left shift
        else if( exp >= 23 )
            return sign ? (-(f << (exp - 23))) : (f << (exp - 23));
        else if( 0 <= exp )
            return sign ? (-(f >> (23 - exp))) : (f >> (23 - exp));
        //exp < 0,round down to 0
        else
            return 0;
    }
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
    //less than minimal value 2^(-23) * 2^(-127)
    if( x < -150 )
        return 0;
    //large than maximum value 2^(127) * (2 - s)
    else if( x > 127 )
        return 0xff << 23;
    //denormal
    else if( x < -126 )
        return 1 << (23 - (-126 - x));
    //normal
    else
        return (x + 127) << 23;
}
