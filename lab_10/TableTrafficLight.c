// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"


// ***** 2. Global Declarations Section *****
enum states { GoN=0, WoN, GoE, WoE, Wa, Hu};

struct State 
{
	void(*FuncPt)(int);			//function pointer
  unsigned long WaitTime;
  unsigned long Next[8];
} ;



// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts




// ***** 3. Subroutines Section *****
void Output(int stt)
{
	switch(stt)
	{
		case(GoN):
			GPIO_PORTB_DATA_R = 0x2;
			GPIO_PORTF_DATA_R = 0x21;
			break;
		
		case(WoN):
			GPIO_PORTB_DATA_R = 0x2;
			GPIO_PORTF_DATA_R = 0x22;
			break;
		
		case(GoE):
			GPIO_PORTB_DATA_R = 0x2;
			GPIO_PORTF_DATA_R = 0xC;
			break;
		
		case(WoE):
			GPIO_PORTB_DATA_R = 0x2;
			GPIO_PORTF_DATA_R = 0x14;
			break;
		
		case(Wa):
			GPIO_PORTB_DATA_R = 0x08;
			GPIO_PORTF_DATA_R = 0x24;
			break;
		
		case(Hu):
			GPIO_PORTB_DATA_R = 0x2;
			GPIO_PORTF_DATA_R = 0x24;
			break;
	}
}


int main(void)
{
	int S= GoN;
	
	struct State FSM[] =
	{
		{&Output, 80000, {GoN, WoN, GoN, WoN, WoN, WoN, WoN, WoN}},
		{&Output, 80000, {GoN, WoN, GoN, WoN, WoN, WoN, WoN, WoN}}
	};

	
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
  
	
  while(1)
		{
			(FSM[S].FuncPt)(S); 

			//SysTick_Wait10ms(FSM[S].WaitTime);
//    Input = SENSOR;     // read sensors
//    S = FSM[S].Next[Input];  
  }
}

