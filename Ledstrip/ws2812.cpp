// ==========================================================================
//
// File      : ws2812.cpp
// Part of   : ws2812 ledstrip library
// Copyright : Jochem van Weelde 2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================
#include "ws2812.hpp"
//===========================================================================
void ws2812led::write(bool x){
    if(x){
        for(int i = 0; i < 24; i++){
            pin.write(1);
            pin.flush();
            wait_busy(1);
            pin.write(pinoutarray[i]);
            pin.flush();
            wait_busy(15);
            pin.write(0);
            pin.flush();
            wait_busy(5);
        }
    }else{
        for(int i = 0; i < 24; i++){
            pin.write(1);
            pin.flush();
            wait_busy(1);
            pin.write(pinoutarrayoff[i]);
            pin.flush();
            wait_busy(15);
            pin.write(0);
            pin.flush();
            wait_busy(5);
        }
    }
    hwlib::wait_us(100);
}

void ws2812led::changecolor(hwlib::color rgbcolor, bool writebit){
    int minus;
    uint8_t rgbvalue;
    int colorcount = 0;
    for(uint8_t x : {rgbcolor.green, rgbcolor.red, rgbcolor.blue}){
        minus = 128;
        rgbvalue = x;
        for(int i = 0; i < 8; i++){
            if(rgbvalue >= minus){
                rgbvalue -= minus;
                if(writebit){
                    pinoutarray[colorcount] = (1);
                }else{
                    pinoutarrayoff[colorcount] = (1);
                }
            }else{
                if(writebit){
                    pinoutarray[colorcount] = (0);
                }else{
                    pinoutarrayoff[colorcount] = (0);
                }
            }
            minus = minus/2;
            colorcount++;
        }
    }
}
//===========================================================================
void ws2812led_invert::write(bool x) {
    ws2812led(pin, rgbcolor).write(!x);
}
//===========================================================================
//====> ws2812ledstrip_dummy does nothing. Empty functions in hpp file
//===========================================================================
void ws2812led_port::write(bool x){
    for( uint_fast8_t i = 0; i < ledcount; i++ ){
        leds[ i ]->write(x);
    }
}
//===========================================================================
void ledstrip_array::write(bool x){
    for(int i=0; i < totalleds; i++){
        bool pinoutarray[24];
        if(x){
            makearray(leds[i].on, pinoutarray);
        }else{
            makearray(leds[i].off, pinoutarray);
        }
        for(int i = 0; i < 24; i++){
            pin.write(1);
            pin.flush();
            wait_busy(1);
            pin.write(pinoutarray[i]);
            pin.flush();
            wait_busy(10);
            pin.write(0);
            pin.flush();
        }
    }
}

void ledstrip_array::changeled(int led, const structws2812 & rgbstruct){
    leds[led-1] = rgbstruct;
}

void ledstrip_array::changegroupled(int beginled, int endled, const structws2812 & rgbstruct){
    for(int i = beginled-1; i < endled; i++){
        leds[i] = rgbstruct;
    }
}

void ledstrip_array::shiftforward(int times){
    for(int j = 0; j < times; j++){
        structws2812 buffer = leds[totalleds-1];
        for(int i = totalleds-1; i > 0; i--){
            leds[i] = leds[i-1];
        }
        leds[0] = buffer;
    }
}

void ledstrip_array::shiftback(int times){
    for(int j = 0; j < times; j++){
        structws2812 buffer = leds[0];
        for(int i = 0; i < totalleds-1; i++){
            leds[i] = leds[i+1];
        }
        leds[totalleds-1] = buffer;
    }
}

void ledstrip_array::snake(int fastness, int countsnakes){
    for(int snakes = 0; snakes < countsnakes; snakes++){
        int ledcount = 0;
        write(0);
        for(int j = 0; j < totalleds; j++){
            ledcount++;
            for(int i=0; i < ledcount; i++){
                bool pinoutarray[24];
                makearray(leds[i].on, pinoutarray);
                for(int i = 0; i < 24; i++){
                    pin.write(1);
                    pin.flush();
                    wait_busy(1);
                    pin.write(pinoutarray[i]);
                    pin.flush();
                    wait_busy(10);
                    pin.write(0);
                    pin.flush();
                }
            }
            hwlib::wait_ms(fastness);
        }
    }
}

int ledstrip_array::countleds(){
    return totalleds;
}
//===========================================================================
void ledstrip_invert::write(bool x){
    ledstrip_array(pin, totalleds).write(!x);
}

void ledstrip_invert::snake(int fastness, int countsnakes){
    for(int snakes = 0; snakes < countsnakes; snakes++){
        int ledcount = 0;
        for(int j = 0; j < totalleds; j++){
            ledcount++;
            for(int i=0; i < ledcount; i++){
                bool pinoutarray[24];
                makearray(leds[i].off, pinoutarray);
                for(int i = 0; i < 24; i++){
                    pin.write(1);
                    pin.flush();
                    wait_busy(1);
                    pin.write(pinoutarray[i]);
                    pin.flush();
                    wait_busy(10);
                    pin.write(0);
                    pin.flush();
                }
            }
            hwlib::wait_ms(fastness);
        }
        write(0);
    }
}
//===========================================================================
void ledstrip_port::write(bool x){
    for( uint_fast8_t i = 0; i < ledstripcount; i++ ){
        ledstrips[ i ]->write(x);
    }
}

void ledstrip_port::changegroupled(int beginled, int endled, const structws2812 & rgbstruct){
    int targetled = beginled;
    int endtarget = endled;
    int ledspassed = 0;
    for( uint_fast8_t i = 0; i < ledstripcount; i++ ){ //loop for all ledstips that are not dummy
        ledspassed += ledstrips[ i ]->countleds();
        if(ledspassed > targetled){ //If led needs to be on, on the selected strip.
            if(ledspassed < endled){
                ledstrips[i]->changegroupled(targetled, ledstrips[ i ]->countleds(), rgbstruct);
                endtarget -=ledstrips[i]->countleds();
                targetled = 1;
            }else{
                ledstrips[i]->changegroupled(targetled, endtarget, rgbstruct);
                break;
            }
        }else{ //De leds on that ledstrip didnt need to be changed.
            targetled -= ledstrips[i]->countleds();
        }
    }
}

int ledstrip_port::countleds(){
    int ledcounter = 0;
    for( uint_fast8_t i = 0; i < ledstripcount; i++ ){
        ledcounter += ledstrips[i]->countleds();
    }
    return ledcounter;
}
//===========================================================================