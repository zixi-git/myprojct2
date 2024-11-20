#ifndef __LED_H__
#define __LED_H__


extern void led_init(void);
extern void led_ctrl(uint32_t n,uint32_t sta);

extern void led_breath_init(void);
extern void led_breath_brightness(uint8_t brightness);
extern void led_breath_run(void);
extern void led_breath_stop(void);

#endif
