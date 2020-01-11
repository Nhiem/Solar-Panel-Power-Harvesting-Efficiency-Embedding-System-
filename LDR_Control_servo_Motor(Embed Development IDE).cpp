#include "mbed.h"

//LDR sensor input 
AnalogIn analogvalue1(PC_5); //0
AnalogIn analogvalue2(PC_0); //1


// Servo Motor 
PwmOut servo (PA_6);
PwmOut servo1 (PA_7);

int main(){



    double topleft; 
    double topright; 
    double downleft; 
    double downright; 
    double rotate_value; 
    double rotate_value1;

    // first Servo Motor 
    servo.period_us(10000);  // 20ms period, typical for analog RC servo motor 
    double servopulsewidth= 1450; 
servo.pulsewidth_us(servopulsewidth); // centers the Servo. Servo range is from 500 to 2400ms
wait_ms(0.2);
// second servo motor  
    servo1.period_us(10000); 
    double servo1pulsewidth= 1450; 
servo.pulsewidth_us(servo1pulsewidth); 
wait_ms(0.2); 

while(1){

    topleft= analogvalue1.read(); 
    topright = analogvalue2.read(); 

    printf("measure LRD_1 = %1.3f\n\r", topleft); 
    printf("measure LRD_2 =  %1.3f\n\r", topright); 

    wait(0.1); // ms second 





      if (topleft !=  topright){
            
         if (topleft < topright){
         	while(topleft < topright){
            topleft = topleft+ 0.01;
            rotate_value = (topleft*(2400)-100);
            servo.pulsewidth_us(rotate_value);
            printf("value_1= %f\r\n", rotate_value );
            if(topleft > topright){
            	 break; 
            	 wait(0.1);
            }
             
            break; 
         }
        }

        if (topleft > topright){
        	while(topleft >topright){
            topright=topright+ 0.01;
           
            rotate_value1 = (topright*(4800)+100);
            servo.pulsewidth_us(rotate_value1); //0 deg
            printf("value_2= %f\r\n", rotate_value1 );
            	if(topleft < topright){
              wait(0.1);
              break; 
         	 	}
         	 	break; 
         	 }

            }

        }
    }
}