// Hello World example for the USBAudio library
 
#include "mbed.h"
// #include "USBAudio.h"
#include "USBHID.h"
#include "QEI.h"

// void drive_motor(int16_t pwm);
void drive_motor(float pwm);

 
QEI wheel (PC_11, PC_12, NC, 64 * 12, QEI::X4_ENCODING);
Serial pc(USBTX, USBRX);


DigitalOut ph(PC_8);
PwmOut en(PC_9);
DigitalOut sleep_pin(PC_10);

int32_t last_pulses = 0;

USBHID hid(8, 8);

//This report will contain data to be sent
HID_REPORT send_report;
HID_REPORT recv_report;
 
DigitalOut l1(LED1);

 
int main() {
    en.period_us(40);
    sleep_pin = 1;
    ph = 0;
    en = 0.00;
    
    while (1) {
        send_report.length = 8;
        memset(send_report.data, 0, sizeof(send_report.data));

        int32_t pulses = wheel.getPulses();
        ((int32_t *) send_report.data)[0] = pulses - last_pulses;
        last_pulses = pulses;

        hid.send(&send_report);

        if(hid.readNB(&recv_report)) {
            l1 = !l1;
            drive_motor(((float *) recv_report.data)[0]);
        }
    
    }
}

void drive_motor(float pwm) {
    en = abs(pwm);
    ph = pwm > 0 ? 1 : 0;
}