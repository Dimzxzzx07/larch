#include <stdint.h>

#define RTC_ADDR 0x70
#define RTC_DATA 0x71

static uint8_t rtc_read(uint8_t reg) {
    __asm__ volatile("outb %0, %1" : : "a"(reg), "d"(RTC_ADDR));
    uint8_t value;
    __asm__ volatile("inb %1, %0" : "=a"(value) : "d"(RTC_DATA));
    return value;
}

static int rtc_bcd_to_bin(uint8_t bcd) {
    return (bcd & 0x0F) + ((bcd >> 4) * 10);
}

void rtc_get_time(int* hour, int* minute, int* second) {
    uint8_t status = rtc_read(0x0B);
    int is_bcd = (status & 0x04) == 0;
    
    *second = rtc_read(0x00);
    *minute = rtc_read(0x02);
    *hour = rtc_read(0x04);
    
    if (is_bcd) {
        *hour = rtc_bcd_to_bin(*hour);
        *minute = rtc_bcd_to_bin(*minute);
        *second = rtc_bcd_to_bin(*second);
    }
}

void cmd_date(void) {
    int hour, minute, second;
    rtc_get_time(&hour, &minute, &second);
    
    vga_writestring("\n");
    char buf[3];
    buf[0] = '0' + (hour / 10);
    buf[1] = '0' + (hour % 10);
    buf[2] = 0;
    vga_writestring(buf);
    vga_writestring(":");
    buf[0] = '0' + (minute / 10);
    buf[1] = '0' + (minute % 10);
    vga_writestring(buf);
    vga_writestring(":");
    buf[0] = '0' + (second / 10);
    buf[1] = '0' + (second % 10);
    vga_writestring(buf);
    vga_writestring("\n");
}