#include "C:\Users\Jules\Documents\Archi\M2101-Archi\Projet-Portail\main.h"

// RFIDs
#define rfid_in input(pin_b0)
#define rfid_out input(pin_b1)

// Moteur
#define motor_clockwise pin_d0
#define motor_counter_clockwise pin_d1

// Capteurs
#define is_open  input(pin_b6)
#define is_close input(pin_b7)
#define infrared input(pin_b3)
#define car_detected input(pin_b2)

//Constante
unsigned int8 OPENING_TIME = 20;

// Variables
char rfid_state=0;
signed int8 tenth=0, open_timer=-1;

#int_TIMER1
void  TIMER1_isr(void) 
{
   set_timer1(3036);// Tout les dix�mes
   tenth++;
   if(tenth==10){// Toute les secondes
      tenth=0;
      if(open_timer>0){
         open_timer--;
         printf("%d \r\n",open_timer);
      }
   }
}

void open(){
   while(is_open){
      output_low(motor_clockwise);
   }
   output_high(motor_clockwise);
   open_timer=OPENING_TIME;
}

void close(){
   while(is_close && !infrared){
      output_low(motor_counter_clockwise);
   }
   output_high(motor_counter_clockwise);
   open_timer=-1;
   if(infrared){
      open();
   }
}

void main()
{
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_wdt(WDT_OFF);
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   setup_timer_2(T2_DISABLED,0,1);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(GLOBAL);
//Setup_Oscillator parameter not selected from Intr Oscillator Config tab

   while(true){
      if((rfid_in||rfid_out||car_detected)&&!(rfid_state)){
         rfid_state=1;
         open();
      }
      if(open_timer==0){
         close();
         rfid_state=0;
      }
   }
}
