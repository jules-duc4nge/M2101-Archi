#include "C:\Users\Jules\Documents\Archi\M2101-Archi\Projet-Alarme\main.h"

//Digicode

//Colonnes
#define digi_column_1 pin_d0
#define digi_column_2 pin_d1
#define digi_column_3 pin_d2

//Lignes
#define digi_row_1 input(pin_d3)
#define digi_row_2 input(pin_d4)
#define digi_row_3 input(pin_d5)
#define digi_row_4 input(pin_d6)

//Alarmes et buzzer
#define buzzer pin_a0
#define alarm pin_b7
#define alarm_led pin_c0

//Fen�tres
#define window_1 input(pin_b1)
#define window_2 input(pin_b2)
#define window_3 input(pin_b3)
#define window_4 input(pin_b4)
#define door_1 input(pin_b5)
#define door_2 input(pin_b6)

//Led fen�tres
#define window_1_led pin_c1
#define window_2_led pin_c2
#define window_3_led pin_c5
#define window_4_led pin_c4
#define door_1_led pin_e0
#define door_2_led pin_e1

//Constantes
int8 const ARMAMENT_CODE=33, const DISARMAMENT_CODE=1664,
   const DELAY_OUT=20,const DELAY_IN=25, const LEGAL_ALARM_TIME=180;
   
//Variables
signed int8 tenth=0, code=0, alarm_delay=-1, alarm_ext_delay=-1, door_delay=-1,
    alarm_state=0, alarm_ext_state=0;
char general_check=(window_1 && window_2 && window_3 && window_4 && door_1 && door_2);
    
//Fonctions et m�thodes
void alarm_ext_setup(){
   alarm_ext_delay=10;//LEGAL_ALARM_TIME;
   alarm_ext_state=1;
   output_high(alarm);
}

void general_reset(){
   output_low(window_1_led);//Fen�tres
   output_low(window_2_led);
   output_low(window_3_led);
   output_low(window_4_led);
   output_low(door_1_led);//Portes
   output_low(door_2_led);
   alarm_delay=-1;//D�lais
   alarm_ext_delay=-1;
   door_delay=-1;
}
   
#int_TIMER1
void TIMER1_isr(void){
   set_timer1(3036); //Tout les dixi�mes
   tenth++;
   if(tenth==10){//Toute les secondes
      tenth=0;
      if(alarm_delay>0){
         alarm_delay--;
      }
      if(alarm_ext_delay>0){
         alarm_ext_delay--;
      }
      if(door_delay>0){
         door_delay--;
      }
   }
}

#int_EXT
void  EXT_isr(void){
   //Lecture de la colone 1
   output_high(digi_column_1);
   output_low(digi_column_2);
   output_low(digi_column_3);
   
   if(digi_row_1){
      printf("1");
      code=10*code+1;
   }
   if(digi_row_2){
      printf("4");
      code=10*code+4;
   }
   if(digi_row_3){
      printf("7");
      code=10*code+7;
   }
   if(digi_row_4){
      printf("%c%c ",254,1);
      code=0;
   }
   
   //Lecture de la colone 2
   output_low(digi_column_1);
   output_high(digi_column_2);
   output_low(digi_column_3);
   
   if(digi_row_1){
      printf("2");
      code=10*code+2;
   }
   if(digi_row_2){
      printf("5");
      code=10*code+5;
   }
   if(digi_row_3){
      printf("8");
      code=10*code+8;
   }
   if(digi_row_4){
      printf("0");
      code=10*code+0;
   }
   
   //Lecture de la colone 3
   output_low(digi_column_1);
   output_low(digi_column_2);
   output_high(digi_column_3);
   
   if(digi_row_1){
      printf("3");
      code=10*code+3;
   }
   if(digi_row_2){
      printf("6");
      code=10*code+6;
   }
   if(digi_row_3){
      printf("9");
      code=10*code+9;
   }
   if(digi_row_4){
      printf("#");
      if(code==ARMAMENT_CODE){
         general_reset();
         //Check-up
         if(general_check){
            //Setup
            alarm_state=0;
            alarm_ext_state=0;
            printf("%c%c Alarme ON",254,1);
            alarm_delay=5;//DELAY_OUT;
            output_high(buzzer);
         }else{
            printf("%c%c Maison ouverte\nAnnulation",254,1);
         }
      }else if(code==DISARMAMENT_CODE){
         printf("%c%c Alarme OFF",254,1);
         alarm_state=-1;
         output_low(buzzer);
         output_low(alarm);
         output_low(alarm_led);
      }else{
         printf("%c%c Code faux",254,1);
      }
      code=0;//Reset du code
   }
   //Reset du digicode
   output_high(digi_column_1);
   output_high(digi_column_2);
   output_high(digi_column_3);
}

void main(){
   setup_adc_ports(NO_ANALOGS);
   setup_adc(ADC_CLOCK_DIV_2);
   setup_psp(PSP_DISABLED);
   setup_spi(SPI_SS_DISABLED);
   setup_wdt(WDT_OFF);
   setup_timer_0(RTCC_INTERNAL);
   setup_timer_1(T1_INTERNAL|T1_DIV_BY_8);
   setup_timer_2(T2_DISABLED,0,1);
   enable_interrupts(INT_TIMER1);
   enable_interrupts(INT_EXT);
   enable_interrupts(GLOBAL);
   //Setup_Oscillator parameter not selected from Intr Oscillator Config tab
   
   //Setup du digicode
   output_high(digi_column_1);
   output_high(digi_column_2);
   output_high(digi_column_3);
   
   while(true){
      if(alarm_delay==0 && alarm_state==0){
         alarm_state=1;
         output_low(buzzer);
         output_high(alarm_led);
      }
      
      if(alarm_ext_state==1 && alarm_ext_delay==0){
            alarm_ext_state=-1;
            output_low(alarm);
      }
      if(door_delay==0 && alarm_state==1){
            if(alarm_ext_state==0)
               alarm_ext_setup();
            if(!door_1){
               printf("%c%c Alerte, porte 1",254,1);
               output_high(door_1_led);
            } else if(!door_2){
               printf("%c%c Alerte, porte 2",254,1);
               output_high(door_2_led);
            }
      }
      
      if(alarm_state == 1){
         if(!window_1){
            printf("%c%c Alerte, fen�tre 1",254,1);
            output_high(window_1_led);
            if(alarm_ext_state==0)
               alarm_ext_setup();
         }else if(!window_2){
            printf("%c%c Alerte, fen�tre 2",254,1);
            output_high(window_2_led);
            if(alarm_ext_state==0)
               alarm_ext_setup();
         }else if(!window_3){
            printf("%c%c Alerte, fen�tre 3",254,1);
            output_high(window_3_led);
            if(alarm_ext_state==0)
               alarm_ext_setup();
         }else if(!window_4){
            printf("%c%c Alerte, fen�tre 4",254,1);
            output_high(window_4_led);
            if(alarm_ext_state==0)
               alarm_ext_setup();
         }else if((!door_1|| !door_2)&&door_delay==-1){
            output_high(buzzer);
            door_delay=5;//DELAY_IN
         }
      }
   }
}
