/*****************************************************************************
 * Brumar Florin - Ilie
 * File name: temp_main.c
 * Date:8/16/2016
 * Description: Temperature conversion testing file
 ******************************************************************************/


#include "display.h"
#include "temperature.h"


static uint32_t msTicks = 0;
static uint8_t buf[10];


static void intToString(int value, uint8_t* pBuf, uint32_t len, uint32_t base)  //converts to string an integer value

{

    static const char* pAscii = "0123456789abcdefghijklmnopqrstuvwxyz";
    int pos = 0;
    int tmpValue = value;

    // the buffer must not be null and at least have a length of 2 to handle one
    // digit and null-terminator
    if (pBuf == NULL || len < 2)
    {
        return;
    }

    // a valid base cannot be less than 2 or larger than 36
    // a base value of 2 means binary representation. A value of 1 would mean only zeros
    // a base larger than 36 can only be used if a larger alphabet were used.
    if (base < 2 || base > 36)
    {
        return;
    }

    // negative value
    if (value < 0)
    {
        tmpValue = -tmpValue;
        value    = -value;
        pBuf[pos++] = '-';
    }

    // calculate the required length of the buffer
    do {
        pos++;
        tmpValue /= base;
    } while(tmpValue > 0);


    if (pos > len)
    {
        // the len parameter is invalid.
        return;
    }

    pBuf[pos] = '\0';

    do {
        pBuf[--pos] = pAscii[value % base];
        value /= base;
    } while(value > 0);

    return;

}
void SysTick_Handler(void) {  //counts the number of clock ticks
    msTicks++;
}

static uint32_t getTicks(void) //return the number of clock ticks
{
    return msTicks;
}
void init_display (void)
{


    GPIOInit();
    init_timer32(0, 10);

    UARTInit(115200);
    UARTSendString((uint8_t*)"OLED - Peripherals\r\n");

    I2CInit( (uint32_t)I2CMASTER, 0 );
    SSPInit();
    ADCInit( ADC_CLK );

     oled_init();
     temp_init (&getTicks);


    /* setup sys Tick. Elapsed time is e.g. needed by temperature sensor */
    SysTick_Config(SystemCoreClock / 1000);
    if ( !(SysTick->CTRL & (1<<SysTick_CTRL_CLKSOURCE_Msk)) )
    {
      /* When external reference clock is used(CLKSOURCE in
      Systick Control and register bit 2 is set to 0), the
      SYSTICKCLKDIV must be a non-zero value and 2.5 times
      faster than the reference clock.
      When core clock, or system AHB clock, is used(CLKSOURCE
      in Systick Control and register bit 2 is set to 1), the
      SYSTICKCLKDIV has no effect to the SYSTICK frequency. See
      more on Systick clock and status register in Cortex-M3
      technical Reference Manual. */
      LPC_SYSCON->SYSTICKCLKDIV = 0x08;
    }



    oled_clearScreen(OLED_COLOR_WHITE);

    oled_putString(1,1,  (uint8_t*)"Temp   : ", OLED_COLOR_BLACK, OLED_COLOR_WHITE);
}
void temp_display(void)
{
	uint32_t t = 0;
        /* Temperature */
        t = temp_conversion(TRUE);
        delay32Ms(0, 3000);

        /* output values to OLED display */

               intToString(t, buf, 10, 10);
               oled_fillRect((1+9*6),1, 80, 8, OLED_COLOR_WHITE);
               oled_putString((1+9*6),1, buf, OLED_COLOR_BLACK, OLED_COLOR_WHITE);

               t = temp_conversion(TRUE);
               delay32Ms(0, 3000);

              intToString(t, buf, 10, 10);
              oled_fillRect((1+9*6),1, 80, 8, OLED_COLOR_WHITE);
              oled_putString((1+9*6),1, buf, OLED_COLOR_BLACK, OLED_COLOR_WHITE);
    }


int main()
{
	  ADCInit( ADC_CLK );
      init_display();

while(1){

		temp_display();
}




}
