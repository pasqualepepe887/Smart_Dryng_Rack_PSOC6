
/*
 * Pasquale Pepe - Smart Drying Rack pasqualepepe887@gmail.com
 *
 */

#include "cy_pdl.h"
#include "cyhal.h"
#include "cybsp.h"
#include <stdio.h>
#include "cy_retarget_io.h"
#include "HCSR04.h"
#include  "string.h"
#include "stdlib.h"

//define HCSR04 Pin
#define TRIGGER_Pin         CYBSP_D9
#define ECHO_Pin            CYBSP_D8

#define TRIGGER2_Pin         CYBSP_D12
#define ECHO2_Pin            CYBSP_D13

#define TRIGGER3_Pin          CYBSP_A0
#define ECHO3_Pin             CYBSP_A1

#define TRIGGER4_Pin         CYBSP_A2
#define ECHO4_Pin            CYBSP_A3

//define RAINSENSOR Pin
#define RAIN_SENSOR           CYBSP_D11

//define L298N Pin L= Left R= Right
#define ML_1               P5_7
#define ML_2              P5_6

#define MR_1              P5_5
#define MR_2              P5_4


void motor(char* name_motor,char* state,int time){
	if (time == 0){
		time = 4000;
	}

	if (strcmp(name_motor, "L")==0){
			if(strcmp(state,"BACK")==0){
			    cyhal_gpio_write(ML_1,false); // ML AVANTI
			    cyhal_gpio_write(ML_2,true);

			} else if (strcmp(state,"FOR")==0){
			    cyhal_gpio_write(ML_1,false); // ML INDIETRO
			    cyhal_gpio_write(ML_2,true);
			} else if (strcmp(state,"STOP")==0){
			    cyhal_gpio_write(ML_1,false); // ML STOP
			    cyhal_gpio_write(ML_2,false);
			}
	}else if(strcmp(name_motor, "R")==0){
		if(strcmp(state,"BACK")==0){
					    cyhal_gpio_write(MR_1,false); // MR AVANTI
					    cyhal_gpio_write(MR_2,true);

					} else if (strcmp(state,"FOR")==0){
					    cyhal_gpio_write(MR_1,true); // MR INDIETRO
					    cyhal_gpio_write(MR_2,false);

					}else if (strcmp(state,"STOP")==0){
					    cyhal_gpio_write(MR_1,false); // MR STOP
					    cyhal_gpio_write(MR_2,false);
					}
	}else if (strcmp(name_motor, "L+R")==0){
		if(strcmp(state,"RIGHT")==0){
							    cyhal_gpio_write(MR_1,false); // MR AVANTI
							    cyhal_gpio_write(MR_2,true);
							    cyhal_gpio_write(ML_1,false); // ML INDIETRO
							    cyhal_gpio_write(ML_2,true);
			        		      Cy_SysLib_Delay(time);
								    cyhal_gpio_write(MR_1,false); // MR STOP
								    cyhal_gpio_write(MR_2,false);
								    cyhal_gpio_write(ML_1,false); // ML STOP
								    cyhal_gpio_write(ML_2,false);

		} else if (strcmp(state,"LEFT")==0){
							    cyhal_gpio_write(ML_1,false); // ML AVANTI
							    cyhal_gpio_write(ML_2,true);
							    cyhal_gpio_write(MR_1,true); // MR INDIETRO
							    cyhal_gpio_write(MR_2,false);
			        		      Cy_SysLib_Delay(time);
								    cyhal_gpio_write(MR_1,false); // MR STOP
								    cyhal_gpio_write(MR_2,false);
								    cyhal_gpio_write(ML_1,false); // ML STOP
								    cyhal_gpio_write(ML_2,false);


							}
	}
}


bool scan_rain(){
	bool state = false;
	bool result_scan= false;

	result_scan= cyhal_gpio_read(RAIN_SENSOR); // Set Variable
	if (result_scan == true){  // First Scan
	    Cy_SysLib_Delay(1000);
		result_scan= cyhal_gpio_read(RAIN_SENSOR);
		if (result_scan == true){ // Second Scan

				state = true; // Set State to True = Detect Rain

			}

		}else {
		state = false;


}
	return state;
}

int main(void)
{
    cy_rslt_t result = cybsp_init();
    if (result != CY_RSLT_SUCCESS) {

        CY_ASSERT(0);
    }

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);

    printf("\x1b[2J\x1b[;H");


    printf("Stendino_Smart!!!\r\n\n");

    if (result != CY_RSLT_SUCCESS)
       {
           CY_ASSERT(0);
       }

// CONFIG HCSR04
    HCSR04_config(TRIGGER_Pin,ECHO_Pin);
    HCSR04_config(TRIGGER2_Pin,ECHO2_Pin);
    HCSR04_config(TRIGGER3_Pin,ECHO3_Pin);
    HCSR04_config(TRIGGER4_Pin,ECHO4_Pin);



    result= cyhal_gpio_init(ML_1, CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, false); //ML_1

    result=  cyhal_gpio_init(ML_2, CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, false); //ML_2

     result=cyhal_gpio_init(MR_1, CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, true); //MR_1

     result =cyhal_gpio_init(MR_2, CYHAL_GPIO_DIR_OUTPUT,CYHAL_GPIO_DRIVE_STRONG, true); //MR_2

     result =cyhal_gpio_init(RAIN_SENSOR, CYHAL_GPIO_DIR_INPUT,CYHAL_GPIO_DRIVE_NONE, true); //RAINSENSOR

    char buffer[50];

      motor("L","STOP",0);
      motor("R","STOP",0);

    for(;;)
    {
        int distance = HCSR04_getdistance(TRIGGER_Pin,ECHO_Pin);
        int distance2 = HCSR04_getdistance(TRIGGER2_Pin,ECHO2_Pin);
        int distance3 = HCSR04_getdistance(TRIGGER3_Pin,ECHO3_Pin);
        int distance4 = HCSR04_getdistance(TRIGGER4_Pin,ECHO4_Pin);

        sprintf(buffer, "HCSR04:[%d/%d/%d/%d]\r\n", distance,distance2,distance3,distance4);
        printf(buffer);

        sprintf(buffer, "RAINSENSOR:[%d]\r\n", cyhal_gpio_read(RAIN_SENSOR));
        printf(buffer);


        if (scan_rain() == true){
        	  printf("Rain!!!\r\n\n");	// Print Rain Message in the Terminal
        	  if (distance <= 10){
        		  if(distance4>= 10){
        			  //gira a destra
        			  motor("L+R","RIGHT",0);


        		  }else  if (distance2<=10){
        				  //gira a sinistra
        			  motor("L+R","LEFT",0);
        			  }

        	  }else{
              motor("L","FOR",0);
              motor("R","FOR",0);
        	   }

        	  // torna a B
        } else {
            motor("L","STOP",0);
            motor("R","STOP",0);
        }

        Cy_SysLib_Delay(1000);
    }
}



