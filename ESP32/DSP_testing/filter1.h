#ifndef FILTER1_H_ // Include guards
#define FILTER1_H_

static const int filter1_numStages = 2;
static const int filter1_coefficientLength = 10;
extern float filter1_coefficients[10];

typedef struct
{
	float state[8];
	float output;
} filter1Type;

typedef struct
{
	float *pInput;
	float *pOutput;
	float *pState;
	float *pCoefficients;
	short count;
} filter1_executionState;


filter1Type *filter1_create( void );
void filter1_destroy( filter1Type *pObject );
 void filter1_init( filter1Type * pThis );
 void filter1_reset( filter1Type * pThis );
#define filter1_writeInput( pThis, input )  \
	filter1_filterBlock( pThis, &(input), &(pThis)->output, 1 );

#define filter1_readOutput( pThis )  \
	(pThis)->output

 int filter1_filterBlock( filter1Type * pThis, float * pInput, float * pOutput, unsigned int count );
#define filter1_outputToFloat( output )  \
	(output)

#define filter1_inputFromFloat( input )  \
	(input)

 void filter1_filterBiquad( filter1_executionState * pExecState );
