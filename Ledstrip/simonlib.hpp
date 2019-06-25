#include <hwlib.hpp>

//THIS FUNCTION IS MADE BY WOUTER VAN OOIJEN REPOSITORY WOVO/GODAFOSS
static void __attribute__((noinline)) wait_busy( int32_t n ){
   __asm volatile(                  
      "   .align 4           \t\n"  
      "1: subs.w  r0, #3     \t\n"  
      "   bgt 1b             \t\n"  
      : : "r" ( n )          // uses (reads) n         
   ); 
}

class w2812led{
private:
    due::pin_out pin;
    hwlib::color rgbcolor;
    hwlib::color off = hwlib::color(0,0,0);
    bool pinoutarray[24];
    bool pinoutarrayoff[24] = {0};
public:
    w2812led(due::pin_out pin, hwlib::color & rgbcolor):
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

    void changecolor(hwlib::color rgbcolor, bool writebit = 1){
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

    void blink(int ms, int  times){
        for(int i; i < times; i++){
            write(1);
            hwlib::wait_ms(ms);
            write(0);
            hwlib::wait_ms(ms);
        }
    }
};

struct structw2812{
    hwlib::color on = hwlib::color(255,255,255);
    hwlib::color off = hwlib::color(0,0,0);
    structw2812(hwlib::color on = hwlib::color(255,255,255), hwlib::color off = hwlib::color(0,0,0)):
    on(on),
    off(off){}
};

class ledstrip_array{
private:
    due::pin_out pin;
    int totalleds;
    structw2812 leds[120];

    bool* makearray(const hwlib::color & rgbcolor, bool pinoutarray[24] = {}){
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
        return pinoutarray;
    }

public:
    ledstrip_array(due::pin_out pin, int totalleds):
    pin( pin ),
    totalleds(totalleds)
    {}

    void write(bool x){
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

    void changeled(int led, structw2812 rgbstruct){
        leds[led] = rgbstruct;
    }
    //Example: The first 10 leds: changegroupled(1,10) Beginning at led 1 ending at led 10
    void changegroupled(int beginled, int endled, structw2812 rgbstruct){
        for(int i = beginled-1; i < endled; i++){
            leds[i] = rgbstruct;
        }
    }

    void shiftforward(int times = 1){
        for(int j = 0; j < times; j++){
            structw2812 buffer = leds[totalleds-1];
            for(int i = totalleds-1; i > 0; i--){
                leds[i] = leds[i-1];
            }
            leds[0] = buffer;
        }
    }

    void shiftback(int times = 1){
        for(int j = 0; j < times; j++){
            structw2812 buffer = leds[0];
            for(int i = 0; i < totalleds-1; i++){
                leds[i] = leds[i+1];
            }
            leds[totalleds-1] = buffer;
        }
    }
};

class ledstrip{
private:
    due::pin_out testpin;
    int ledcount;
public:

    ledstrip(due::pin_out testpin, int ledcount):
    testpin( testpin ),
    ledcount( ledcount )
    {}

    void fillleds(const hwlib::color & rgbcolor, int count, bool refreshall = true){
        //auto testpin = hwlib::target::pin_out( hwlib::target::pins::d7 );
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

int getafstand(due::pin_out test, due::pin_in test2){
    test.write(1);
    test.flush();
    hwlib::wait_us(20);
    test.write(0);
    test.flush();
    int counter = 0;
    while(test2.read() == 0){continue;}
    while(test2.read()){
        counter++;
        if(counter > 5000){
            return counter;
        }
    }
    return counter;
}