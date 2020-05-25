#include "cy_pdl.h" // include Peripheral Driver Library
#include "cyhal.h" // include Hardware Abstraction Layer library
#include "cycfg.h" // include device configurator library
/*******************************************************************************
* Function Prototypes
********************************************************************************/
static void button_intr_handler(void *handler_arg, cyhal_gpio_event_t event);
static void wtr_sense_intr_handler(void *handler_arg, cyhal_gpio_event_t event);
/*******************************************************************************
* Global Variables
********************************************************************************/
volatile bool button_intr_flag = false; // Boolean type variable to store the interrupt flag
volatile bool wtr_sense_intr_flag = false; // Boolean type variable to store the interrupt flag
int main(void)
{
    init_cycfg_all(); // configure pins as done in Device Configurator
    __enable_irq(); // enable interrupt
    cyhal_gpio_register_callback(button_HAL_PORT_PIN,
                                 button_intr_handler, NULL); // assigning isr handler for button
    cyhal_gpio_enable_event(button_HAL_PORT_PIN, CYHAL_GPIO_IRQ_RISE,
                                 1u, true); // interrupt on rising edge (button pressed)
    cyhal_gpio_register_callback(wtr_sense_HAL_PORT_PIN,
                                 wtr_sense_intr_handler, NULL); // assigning isr handler for water sensor
    cyhal_gpio_enable_event(wtr_sense_HAL_PORT_PIN, CYHAL_GPIO_IRQ_FALL,
                                 1u, true); // interrupt on falling edge (water level high)

    for (;;)
    {
     if (true == button_intr_flag) // if interrupt happens (button pressed)
     {
         button_intr_flag = false; // clear interrupt flag for further interrupt
         if(Cy_GPIO_ReadOut(water_pump_PORT, water_pump_NUM)==0UL)
         {
          Cy_GPIO_Write(green_PORT, green_NUM, 1UL); // green LED on
          Cy_GPIO_Write(water_pump_PORT, water_pump_NUM, 1UL); // on water pump
         }
         else
         {
          Cy_GPIO_Write(green_PORT, green_NUM, 0UL); // green LED off
          Cy_GPIO_Write(water_pump_PORT, water_pump_NUM, 0UL); // off water pump
         }
     }
     else;
     if(0UL == Cy_GPIO_Read(wtr_sense_PORT, wtr_sense_NUM))
	 {
         Cy_GPIO_Write(red_PORT, red_NUM, 1UL); // red LED on
         Cy_GPIO_Write(green_PORT, green_NUM, 0UL); // green LED off
         Cy_GPIO_Write(water_pump_PORT, water_pump_NUM, 0UL); // off water pump

	 }
     else
     {
         Cy_GPIO_Write(red_PORT, red_NUM, 0UL); // red LED off
     }
     if( true == wtr_sense_intr_flag) // buzzer on for 0.5s
     {
         Cy_GPIO_Write(buzzer_PORT, buzzer_NUM, 1UL); // buzzer on
         cyhal_system_delay_ms(100); // delay 100 ms
         Cy_GPIO_Write(buzzer_PORT, buzzer_NUM, 0UL); // buzzer off
         wtr_sense_intr_flag = false;
     }
    }
}
/*******************************************************************************
* Function Name: button_intr_handler
********************************************************************************
* Summary:
*   button GPIO interrupt handler.
*
* Parameters:
*  void *handler_arg (unused)
*  cyhal_gpio_irq_event_t (unused)
*
*******************************************************************************/
static void button_intr_handler(void *handler_arg, cyhal_gpio_irq_event_t event)
{
    button_intr_flag = true;
}
/*******************************************************************************
* Function Name: wtr_sense_intr_handler
********************************************************************************
* Summary:
*   water sensor GPIO interrupt handler.
*
* Parameters:
*  void *handler_arg (unused)
*  cyhal_gpio_irq_event_t (unused)
*
*******************************************************************************/
static void wtr_sense_intr_handler(void *handler_arg, cyhal_gpio_irq_event_t event)
{
    wtr_sense_intr_flag = true;
}
