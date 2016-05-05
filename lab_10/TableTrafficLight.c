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

#define SENSORs (*((volatile unsigned long *)0x4002401C)) //PortE.0-2
#define Ttrans 		200   //transition time
#define	Tflash		30		//flash time
#define Nflash	  5     //number of hurry flashes

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

void Init(void)
{
	//Set Clock
	volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x32;      // 1) B E F
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  
	//Set PortE (Input PE.0-2)
	GPIO_PORTE_AMSEL_R &= ~0x07; 			// 3) disable analog function
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO
  GPIO_PORTE_DIR_R &= ~0x07;   			// 5) inputs
  GPIO_PORTE_AFSEL_R &= ~0x07; 			// 6) regular function 
  GPIO_PORTE_DEN_R |= 0x07;    			// 7) enable digital
  
	//Set PortB (Output PE.0-5)
	GPIO_PORTB_AMSEL_R &= ~0x3F; 			// 3) disable analog function
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO
  GPIO_PORTB_DIR_R |= 0x3F;    			// 5) outputs
  GPIO_PORTB_AFSEL_R &= ~0x3F; 			// 6) regular function
  GPIO_PORTB_DEN_R |= 0x3F;    			// 7) enable digital
	
	//Set PortF (Output PF1-3)
	GPIO_PORTF_AMSEL_R &= ~0x0E; 			// 3) disable analog function
  GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // 4) enable regular GPIO
  GPIO_PORTF_DIR_R |= 0x0E;   			// 5) outputs
  GPIO_PORTF_AFSEL_R &= ~0x0E; 			// 6) regular function
  GPIO_PORTF_DEN_R |= 0x0E;    			// 7) enable digital
	
	
	//SysTick Initialization
	NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;      // enable SysTick with core clock
}


void SysTick_Delay(unsigned long p_delay)
{
	NVIC_ST_RELOAD_R = p_delay-1;  					// number of counts to wait
  NVIC_ST_CURRENT_R = 0;       						// any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){} // wait for count flag
}

// 10000us equals 10ms
void Delay(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Delay(800000);  // wait 10ms
  }
}





// ***** 3. Subroutines Section *****
void Output(int stt)
{
	unsigned short i;
	
	switch(stt)
	{
		case(GoN):
			GPIO_PORTB_DATA_R = 0x21;
			GPIO_PORTF_DATA_R = 0x2;
			break;
		
		case(WoN):
			GPIO_PORTB_DATA_R = 0x22;
			GPIO_PORTF_DATA_R = 0x2;
			break;
		
		case(GoE):
			GPIO_PORTB_DATA_R = 0xC;
			GPIO_PORTF_DATA_R = 0x2;
			break;
		
		case(WoE):
			GPIO_PORTB_DATA_R = 0x14;
			GPIO_PORTF_DATA_R = 0x2;
			break;
		
		case(Wa):
			GPIO_PORTB_DATA_R = 0x24;
			GPIO_PORTF_DATA_R = 0x08;
			break;
		
		case(Hu):
			GPIO_PORTB_DATA_R = 0x24;
			GPIO_PORTF_DATA_R = 0x2;
			for(i=0; i<=Nflash; i++)
			{
				Delay(Tflash);
				GPIO_PORTF_DATA_R ^= 0x2;
			}
			break;
	}
}


int main(void)
{
	int S= GoN;
	unsigned long Input;
	
	struct State FSM[] =
	{
		{&Output, Ttrans, {GoN, WoN, GoN, WoN, WoN, WoN, WoN, WoN}},		//GoN State
		{&Output, Ttrans, { Wa, GoE,  Wa, GoE,  Wa, GoE,  Wa, GoE}},		//WoN State
		{&Output, Ttrans, {GoE, GoE, WoE, WoE, WoE, WoE, WoE, WoE}},		//GoE State
		{&Output, Ttrans, {GoN, GoN, GoN, GoN,  Wa,  Wa,  Wa,  Wa}},		//WoE State
		{&Output, Ttrans, { Wa,  Hu,  Hu,  Hu,  Wa,  Hu,  Hu,  Hu}},		//Wa	State
		{&Output, Ttrans, {GoE, GoE, GoN, GoE, GoN, GoE, GoN, GoN}},		//Hu	State
	};

	Init();
	
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
  EnableInterrupts();
  
	
  while(1)
	{
				
		(FSM[S].FuncPt)(S); 			//run output function f(S)
		Delay(FSM[S].WaitTime); 	//wait
		Input = SENSORs;     			//read sensors
		S = FSM[S].Next[Input];  	//go to next state
			
//		//Debugging Output
//		Input = SENSORs;     						
//		(FSM[S].FuncPt)(Input);
  }
}

