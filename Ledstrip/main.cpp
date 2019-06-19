#include <hwlib.hpp>
#include "HCSR04.h"
namespace target = hwlib::target;

//THIS FUNCTION IS MADE BY WOUTER VAN OOIJEN REPOSITORY WOVO/GODAFOSS
static void __attribute__((noinline)) wait_busy( int32_t n ){
   __asm volatile(                  
      "   .align 4           \t\n"  
      "1: subs.w  r0, #3     \t\n"  
      "   bgt 1b             \t\n"  
      : : "r" ( n )          // uses (reads) n         
   ); 
}

class w2812{
private:
    hwlib::pin_out& pin;
    hwlib::color rgbcolor;
    hwlib::color off = hwlib::color(0,0,0);
    bool pinoutarray[24];
public:
    w2812(hwlib::pin_out& pin, hwlib::color & rgbcolor):
    pin( pin ),
    rgbcolor( rgbcolor ){
        int minus;
        uint8_t rgbvalue;
        int colorcount = 0;
        for(uint8_t x : {rgbcolor.green, rgbcolor.red, rgbcolor.blue}){
            minus = 128;
            rgbvalue = x;
            for(int i = 0; i < 8; i++){
                if(rgbvalue >= minus){
                    rgbvalue -= minus;
                    pinoutarray[colorcount] = (1);
                }else{
                    pinoutarray[colorcount] = (0);
                }
                minus = minus/2;
                colorcount++;
            }
        }
    }


    void write(bool x){
        bool pinoutarrayb[24] = {pinoutarray};
        if(x){
            for(int i = 0; i < 24; i++){
                pin.write(1);
                pin.flush();
                wait_busy(1);
                pin.write(pinoutarrayb[i]);
                pin.flush();
                wait_busy(10);
                pin.write(0);
                pin.flush();
            }
        }
    }
};

class ledstrip{
private:
    hwlib::pin_out& pin;
    int ledcount;
public:

    ledstrip(hwlib::pin_out& pin, int ledcount):
    pin( pin ),
    ledcount( ledcount )
    {}

    void fillleds(const hwlib::color & rgbcolor, int count, bool refreshall = true){
        auto testpin = hwlib::target::pin_out( hwlib::target::pins::d7 );
        int minus;
        uint8_t rgbvalue;
        bool pinoutarray[24] = {};
        int colorcount = 0;
        for(uint8_t x : {rgbcolor.green, rgbcolor.red, rgbcolor.blue}){
            minus = 128;
            rgbvalue = x;
            for(int i = 0; i < 8; i++){
                if(rgbvalue >= minus){
                    rgbvalue -= minus;
                    pinoutarray[colorcount] = (1);
                }else{
                    pinoutarray[colorcount] = (0);
                }
                minus = minus/2;
                colorcount++;
            }
        }
        for(int j = 0; j < count; j++){
            for(int i = 0; i < 24; i++){
                testpin.write(1);
                testpin.flush();
                wait_busy(1);
                testpin.write(pinoutarray[i]);
                testpin.flush();
                wait_busy(10);
                testpin.write(0);
                testpin.flush();
            }
        }
        if(refreshall){
            for(int j = 0; j < ledcount-count; j++){
                for(int i = 0; i < 24; i++){
                    testpin.write(1);
                    testpin.flush();
                    wait_busy(1);
                    testpin.write(0);
                    testpin.flush();
                    wait_busy(10);
                    testpin.write(0);
                    testpin.flush();
                }
            }
        }
    }
        
    //fastness(us per colorshift. 5000 recomended)
    void rainbow(int fastness, int rainbows){
        for(int j = 0; j < rainbows; j++){
            for(int i = 0; i < 256; i++){
                fillleds(hwlib::color(255-i,i,0),ledcount);
                hwlib::wait_us(fastness);
            }
            for(int i = 0; i < 256; i++){
                fillleds(hwlib::color(0,255-i,i),ledcount);
                hwlib::wait_us(fastness); 
            }
            for(int i = 0; i < 256; i++){
                fillleds(hwlib::color(i,0,255-i),ledcount);
                hwlib::wait_us(fastness); 
            }
        }
    }

    void oneled(const hwlib::color & rgbcolor, int lednumber){
        auto testpin = hwlib::target::pin_out( hwlib::target::pins::d7 );
        fillleds(hwlib::color(0,0,0), lednumber-1);
        fillleds(rgbcolor, 1);
        fillleds(hwlib::color(0,0,0), ledcount - lednumber);
    }

    void addleds(const hwlib::color & rgbcolor, int leds){
        fillleds(rgbcolor, leds, 0);
    }

    void snake(const hwlib::color & rgbcolor, int fastness, int snakes, bool reverse = false){
        for(int j = 0; j < snakes; j++){
            for(int i = 0; i < ledcount; i++){
                fillleds(rgbcolor,i);
                hwlib::wait_ms(fastness);
            }
            if(reverse){
                for(int i = 0; i < ledcount; i++){
                    fillleds(rgbcolor,ledcount-i);
                    hwlib::wait_ms(fastness);
                }
            }
            fillleds(hwlib::color(0,0,0), ledcount);
        }
    }

    void flow(const hwlib::color & rgbcolor, int ledson, int ledsoff, int fastness, int flows){
        for(int h = 0; h < flows * (ledcount/(ledson + ledsoff)); h++){
            for(int i = 0; i < (ledsoff+ledson); i++){
                addleds(hwlib::color(0,0,0),i);
                for(int j = 0; j < (ledcount/(ledson + ledsoff)); j++){
                    addleds(rgbcolor, ledson);
                    addleds(hwlib::color(0,0,0), ledsoff);
                }
                hwlib::wait_ms(200);
            }
        }
    }

    //Color, fastness(ms per led), kits
    void kitt(const hwlib::color & rgbcolor, int fastness, int kits){
        for(int j = 0; j < kits; j++){
            for(int i = 0; i < ledcount; i++){
                oneled(rgbcolor, i);
                hwlib::wait_ms(fastness);
            }
            for(int i = 0; i < ledcount; i++){
                oneled(rgbcolor, ledcount-i);
                hwlib::wait_ms(fastness);
            }
        }
    }

    void disco(){
        for(int i = 0; i < ledcount; i++){
            fillleds(hwlib::color(rand()%256,rand()%256,rand()%256),1);
        }
        hwlib::wait_ms(2000);
        for(int i = 0; i < ledcount; i++){
            fillleds(hwlib::color(rand()%256,rand()%256,rand()%256),60);
            hwlib::wait_ms(100);
        }
        snake(hwlib::color(rand()%256,rand()%256,rand()%256), 20, 1);
        snake(hwlib::color(rand()%256,rand()%256,rand()%256), 20, 1);
        kitt(hwlib::color(rand()%256,rand()%256,rand()%256), 20,1);
        kitt(hwlib::color(rand()%256,rand()%256,rand()%256), 20,1);
        rainbow(500, 2);
    }

};

int main(void){
    hwlib::wait_ms(1000);
    auto pin = hwlib::target::pin_out( hwlib::target::pins::d6 );
    ledstrip leds(pin, 60);
    hwlib::color wit(255,255,255);
    hwlib::color rood(255,0,0);
    hwlib::color groen(0,255,0);
    hwlib::color blauw(0,0,255);
    hwlib::color uit(0,0,0);
    w2812 ledje(pin, blauw);
    for(;;){
        ledje.write(1);
    }
    
}