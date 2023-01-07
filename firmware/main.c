#include <stdio.h>
#include "sam/gpio.h"
#include "sam/bod.h"
#include "sam/clock.h"
#include "sam/sercom_usart.h"
#include "sam/pinmux.h"
#include "sam/serial_number.h"

#include "tusb.h"

void USB_Handler() {
	tud_int_handler(0);
}

volatile uint32_t system_ticks = 0;
void SysTick_Handler (void)
{
  system_ticks++;
}

const int LED0 = 27;
const int LED1 = 22;
const int LED2 = 19;
const int LED3 = 18;

const int BUTTON = 11;

bool current_output = true;

static void switch_to_output(bool out) {
	current_output = out;
	if(current_output) {
		port_set_value(1, true);
		port_set_value(LED0, true);
		port_set_value(LED3, false);
	} else {
		port_set_value(1, false);
		port_set_value(LED0, false);
		port_set_value(LED3, true);
	}
}

static void cdc_task(void)
{
	if ( tud_cdc_n_available(0) )
	{
		uint8_t buf[64];
		uint32_t count = tud_cdc_n_read(0, buf, sizeof(buf));

		for(uint32_t i = 0; i < count; i++) {
			if(buf[i] == '1') {
				switch_to_output(true); // switch to 1
			} else if(buf[i] == '2') {
				switch_to_output(false); // switch to 2
			}
		}
	}
}

static void button_task(void) {
	static int last_tick = 0;
	static bool waiting_for_low = false;

	bool button_value = !port_get_value(BUTTON);
	if(button_value && !waiting_for_low) {
		if(system_ticks - last_tick > 100) {
			last_tick = system_ticks;
			waiting_for_low = true;
			switch_to_output(!current_output);
		}
	} else if(!button_value) {
		waiting_for_low = false;
	}
}

extern char const* string_desc_arr [];

int main() {
	clock_switch_to_8mhz();
	bod_init();
	bod_set_3v3();

	clock_switch_to_48mhz_from_usb();
	clock_setup_gclk2_8mhz();
	clock_setup_usb();

	clock_setup_systick_1ms();

	port_set_direction(0, true); // oeb
	port_set_direction(1, true); //sel1
	port_set_direction(2, true); //rout_s0
	port_set_direction(3, true); // oc_s0
	port_set_direction(4, true); // eq_s0

	port_set_direction(7, false); // hpd

	port_set_direction(LED0, true); // led0
	port_set_direction(LED1, true); // led1
	port_set_direction(LED2, true); // led2
	port_set_direction(LED3, true); // led3

	port_set_value(LED0, true); // led0
	port_set_value(LED1, false); // led1
	port_set_value(LED2, false); // led2
	port_set_value(LED3, false); // led3

	//button
	port_set_direction(11, false);

	port_set_value(0, false); // oeb low = enable
	port_set_value(1, true); // sel1 high = port 1 select
	port_set_value(2, true); // rout_s0 high = 50ohm termination
	port_set_value(3, false); // low = 0db pre-emphasis
	port_set_value(4, false); // low = 9db eq

	pinmux_setup_usb();

	string_desc_arr[3] = (const char *) serial_get_hash_hex();
	//string_desc_arr[3] = "123456";

	tusb_init();
	tud_init(0);

	while(true) {
		tud_task();
		cdc_task();
		button_task();
	}

	return 0;
}
