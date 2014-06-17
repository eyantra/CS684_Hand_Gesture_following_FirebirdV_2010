//
//These are bit functions to set pins of ports
//typical usage is
// bit_fn(PORT,PIN)

//Bit Function definitions
# define tbi(x,y) ( (x) ^= (1<<(y)) )
# define sbi(x,y) ( (x) |= (1<<(y)) )
# define cbi(x,y) ( (x) &= ~(1<<(y)) )
# define isHigh(x,y) (x & (1<<y))


//Variable to see if file is already included
# define BIT_FUNCTION_DEFINED
