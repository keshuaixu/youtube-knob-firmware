// Hello World example for the USBAudio library
 
#include "mbed.h"
#include "USBAudio.h"
#include "QEI.h"

void on_audio_sample();
void drive_motor(int16_t pwm);

 
QEI wheel (PC_11, PC_12, NC, 64 * 12, QEI::X4_ENCODING);
Serial pc(USBTX, USBRX);

DigitalOut ph(PC_8);
PwmOut en(PC_9);
DigitalOut sleep_pin(PC_10);

Ticker every_audio_sample;
 

// length of an audio packet: each ms, we receive 48 * 16bits ->48 * 2 bytes. as there is one channel, the length will be 48 * 2 * 1
const uint16_t AUDIO_LENGTH_PACKET_IN = 2 * 8 * 1;
const uint16_t AUDIO_LENGTH_PACKET_OUT = 2 * 8 * 1;

 
// USBAudio
// USBAudio audio(FREQ, NB_CHA);
// USBAudio (uint32_t frequency_in=48000, uint8_t channel_nb_in=1, uint32_t frequency_out=8000, uint8_t channel_nb_out=1, uint16_t vendor_id=0x7bb8, uint16_t product_id=0x1111, uint16_t product_release=0x0100)
// USBAudio audio(48000, 2, 8000, 1);
USBAudio audio(8000, 1, 8000, 1);

uint8_t sample_index = AUDIO_LENGTH_PACKET_IN;
// float volume = 0.5;
int16_t buf_in[AUDIO_LENGTH_PACKET_IN/2];
int16_t buf_out[AUDIO_LENGTH_PACKET_OUT/2];

int32_t last_pulses = 0;

 
int main() {
    en.period_us(40);
    sleep_pin = 1;
    ph = 0;
    en = 0.00;


    // while(1){
    //     wait(0.1);
    //     // pc.printf("jhsjhs\n");
    //     pc.printf("Pulses is: %i\n", wheel.getPulses());
    // }

    every_audio_sample.attach(&on_audio_sample, 1e-3/8.);
    
    while (1) {
        // read an audio packet

        int32_t pulses = wheel.getPulses();
        buf_out[0] = (int16_t) (pulses - last_pulses);
        audio.readWrite((uint8_t *)buf_in, (uint8_t *)buf_out);
        sample_index = 0;
        last_pulses = pulses;

        // volume = audio.getVolume();
 
        // print packet received
        // pc.printf("recv: ");
        // for(int i = 0; i < AUDIO_LENGTH_PACKET/2; i++) {
        //     pc.printf("%d ", buf[i]);
        // }
    }
}

void on_audio_sample() {
    if (sample_index > AUDIO_LENGTH_PACKET_IN/2 - 1) return;
    drive_motor(buf_in[sample_index]);
    sample_index += 1;    
}

void drive_motor(int16_t pwm) {
    en = abs(pwm) / 32767.0;
    ph = pwm > 0 ? 1 : 0;
}