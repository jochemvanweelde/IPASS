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
/// \project
/// \mainpage w2812 Ledstrip
/// 

/// \brief 
/// w2812led ADT
/// \details
/// This ADT will store 1 w2812led and lets the user control it with multiple functions.
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

    /// \brief 
    /// Write a 1(on) or 0(off) to the led.
    /// \details
    /// write(1) will result in the led turning to the color given in the construcor or with the function changecolor().\n
    /// write(0) will result in the led turning off or the given color in the function changecolor().
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
    /// \brief 
    /// Change the color of the led.
    /// \details
    /// changecolor(hwlib::color()) changes the color of the led to the color that is given.\n
    /// It's also possible to change the 'off-color' with changecolor(hwlib::color(), 0). \n
    /// If you write a 0 to the led, it will change to that color.
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
    /// \brief 
    /// The led will blink on and off.
    /// \details
    /// The led writes an 1 and a 0 for a couple of times.\n
    /// int ms is the duraion between turning the led on and off.\n
    /// int times is how many times the led is going to be turned on.
    void blink(int ms, int  times){
        for(int i; i < times; i++){
            write(1);
            hwlib::wait_ms(ms);
            write(0);
            hwlib::wait_ms(ms);
        }
    }
};

/// \brief 
/// structw2812 (struct)
/// \details
/// This struct is used for ADT ledstrip_array.\n
/// It stores information of 1 led: The 'on color' and the 'off-color'.\n
/// Create a color combination with: structw2812 NAME(hwlib::color(), hwlib::color());\n
/// The standard value is white(on) and black(off).\n
/// For more information, go to ledstrip_array -> changeled() or changegroupled()
struct structw2812{
    hwlib::color on = hwlib::color(255,255,255);
    hwlib::color off = hwlib::color(0,0,0);
    structw2812(hwlib::color on = hwlib::color(255,255,255), hwlib::color off = hwlib::color(0,0,0)):
    on(on),
    off(off){}
};
/// \brief 
/// ledstrip with array (w2812)
/// \details
/// This ADT lets the user store a ledstrip and control it.\n
/// This ADT can store a max of 120 leds on a ledstrip.\n
/// NOTE: The max can be changed by changing the value of the struct length.
class ledstrip_array{
protected:
    due::pin_out pin;

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
    int totalleds;
    structw2812 leds[120];
    ledstrip_array(due::pin_out pin, int totalleds):
    pin( pin ),
    totalleds(totalleds)
    {}
    /// \brief 
    /// Write a 1 or a 0 to the whole ledstrip.
    /// \details
    /// write(1) will result in putting every color stored in the structw2812 array on the ledstrip. (Variable on)\n
    /// write(0) will result in putting every color stored in the structw2812 array on the ledstrip. (Variable off)\n
    /// To change the colors go to changeled() for 1 led or changegroupled() for more.
    virtual void write(bool x){
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
    /// \brief 
    /// Change the color of 1 led
    /// \details
    /// Select the led beginning at 1 and select the color combination.\n
    /// Led 0 does nog exist, de ledstrip starts at 1.
    virtual void changeled(int led, const structw2812 & rgbstruct){
        leds[led-1] = rgbstruct;
    }
    /// \brief 
    /// Change the color of more leds.
    /// \details
    /// Beginning at 1 and ending at 120 (for a ledstrip with 120 leds).\n
    /// The endled will also be included. Led 0 does nog exist, de ledstrip starts at 1.
    virtual void changegroupled(int beginled, int endled, const structw2812 & rgbstruct){
        for(int i = beginled-1; i < endled; i++){
            leds[i] = rgbstruct;
        }
    }
    /// \brief 
    /// Shift all colors forward.
    /// \details
    /// Look for an arraw on your ledstrip.\n
    /// This function moves the colors in the direction of the arrows.\n
    /// shiftforward(3) will shift all the colors 3 leds forward.
    virtual void shiftforward(int times = 1){
        for(int j = 0; j < times; j++){
            structw2812 buffer = leds[totalleds-1];
            for(int i = totalleds-1; i > 0; i--){
                leds[i] = leds[i-1];
            }
            leds[0] = buffer;
        }
    }
    /// \brief 
    /// Shift all colors backwards.
    /// \details
    /// Look for an arraw on your ledstrip.\n
    /// This function moves the colors in the opposite direction of the arrows.\n
    /// shiftback(3) will shift all the colors 3 leds backward.
    virtual void shiftback(int times = 1){
        for(int j = 0; j < times; j++){
            structw2812 buffer = leds[0];
            for(int i = 0; i < totalleds-1; i++){
                leds[i] = leds[i+1];
            }
            leds[totalleds-1] = buffer;
        }
    }
    /// \brief 
    /// Snake animation
    /// \details
    /// This function adds 1 led at a time\n
    /// For example: snake(50, 2) will result in:\n
    /// 2 snakes and every led will be added  each 50 miliseconds.\n
    /// Animation: led 1, led 1 and 2, led 1 and 2 and 3 etc.\n
    /// The leds wont be turned off at the beginning of this animation. 
    virtual void snake(int fastness, int countsnakes){
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

};

/// \brief
/// ledstrip_dummy does nothing
/// \details
/// Like litterally.... nothing.\n
/// All functions returns nothing and do nothing.
class ledstrip_dummy : public ledstrip_array{
public:
    ledstrip_dummy(due::pin_out pin, int totalleds):
    ledstrip_array(pin, totalleds){}
    void write(bool x) override{}
    void changeled(int led, const structw2812 & rgbstruct) override{}
    void changegroupled(int beginled, int endled, const structw2812 & rgbstruct)override{}
    void shiftforward(int times = 1)override{}
    void shiftback(int times = 1)override{}
    void snake(int fastness, int countsnakes)override{}
};

class ledstrip_invert : public ledstrip_array{
public:
    ledstrip_invert(due::pin_out & pin, int totalleds):
    ledstrip_array(pin, totalleds){
    }

    void write(bool x) override {
        ledstrip_array(pin, totalleds).write(!x);
    }
    void snake(int fastness, int countsnakes) override{
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
};

/// \brief
/// Ledstripport for using multiple ledstrips as one.
/// \details
/// Add multiple ledstrippins max(2 ledstrippins) and min(2 ledstrippins)
/// This class works with ledstrip_array
class ledstripport{
private:
    ledstrip_array & ledstrip1;
    ledstrip_array & ledstrip2;
    // ledstrip_array ledstrip3 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip4 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip5 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip6 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip7 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip8 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip9 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip10 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip11 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip12 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip13 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip14 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip15 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    // ledstrip_array ledstrip16 = ledstrip_dummy(due::pin_out( due::pins::d0),0);
    structw2812 leds[240];

    void fillarrays(){
        for(int i = 0; i < ledstrip1.totalleds; i++){
            ledstrip1.leds[i] = leds[i];
        }
        for(int i = 0; i < ledstrip2.totalleds; i++){
            ledstrip2.leds[i] = leds[i + ledstrip1.totalleds];
        }
    }
public:
    ledstripport(ledstrip_array & ledstrip1, ledstrip_array & ledstrip2):
    ledstrip1(ledstrip1),
    ledstrip2(ledstrip2){}

    /// \brief
    /// Turn all the ledsrips on or off.
    /// \details
    /// Turn all the ledstrips to the on-color or the off-color.
    void write(bool x) {
        ledstrip1.write(x);
        ledstrip2.write(x);
    }
    /// \brief
    /// Change the colors in groups.
    /// \details
    /// Change the colors of the 2 ledstrips.\n
    /// This functions changes the structs of the 2 ledstrips.
    void changegroupled(int beginled, int endled, structw2812 rgbstruct){
        for(int i = beginled-1; i < endled; i++){
            leds[i] = rgbstruct;
        }
        fillarrays();
    }
    /// \brief
    /// Snake animation on all ledstrips
    /// \details
    /// This function adds 1 led at a time\n
    /// For example: snake(50, 2) will result in:\n
    /// 2 snakes and every led will be added  each 50 miliseconds.\n
    /// Animation: led 1, led 1 and 2, led 1 and 2 and 3 etc.\n
    /// The leds wont be turned off at the beginning of this animation.
    void snake(int fastness, int countsnakes){
        for(int i = 0; i < countsnakes; i++){
            ledstrip1.snake(fastness, 1);
            ledstrip2.snake(fastness, 1);
            ledstrip1.write(0);
            ledstrip2.write(0);
        }
    }

    void blink(int fastness, int countblinks){
        for(int i = 0; i < countblinks; i++){
            ledstrip1.write(1);
            ledstrip2.write(1);
            hwlib::wait_ms(fastness);
            ledstrip1.write(0);
            ledstrip2.write(0);
            hwlib::wait_ms(fastness);
        }
    }
};



/// \brief 
/// ledstrip ADT for animations
/// \details
/// This ADT lets you control a ledstrip with very easy functions.\n
/// This ADT is focussed on animations. \n
/// If you want to use advanced functions please refer to ledstrip_array or ledstripport.\n
/// NOTE: There is NO limitation of ledcount.
class ledstrip{
private:
    due::pin_out testpin;
    int ledcount;
public:

    ledstrip(due::pin_out testpin, int ledcount):
    testpin( testpin ),
    ledcount( ledcount )
    {}
    /// \brief 
    /// Fill the first couple leds with a given color
    /// \details
    /// Example: fillleds(hwlib::color(255,0,0), 40)\n
    /// The first 40 leds will turn red, the following leds will be turned off. 
    /// \n\n\n\n More colors?
    /// It is also possible to place colors back to back:\n
    /// fillleds(hwlib::color(255,0,0), 40, 0) wont turn off the leds after the 40 red leds.\n
    /// Directly after needs to be another fillleds() to turn the rest of the leds another color.\n
    /// The time between the funtions needs to be less then 50 us.
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
        
    /// \brief 
    /// Rainbow animation
    /// \details
    /// This functions creates a rainbow effect by cycling through all colors.\n
    /// An int fastness of 5000 is recommended. Lowering it by a couple hundred will result in a faster cycle.\n
    /// Int rainbows is the total of cycles.
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
    /// \brief 
    /// Fills one led
    /// \details
    /// This function sets 1 led to a color.
    void oneled(const hwlib::color & rgbcolor, int lednumber){
        fillleds(hwlib::color(0,0,0), lednumber-1);
        fillleds(rgbcolor, 1);
        fillleds(hwlib::color(0,0,0), ledcount - lednumber);
    }
    /// \brief 
    /// Add multiple colored leds
    /// \details
    /// Add multiple colored leds with this function.\n
    /// To do this, the user needs to place addleds() functions directly after eachother.
    void addleds(const hwlib::color & rgbcolor, int leds){
        fillleds(rgbcolor, leds, 0);
    }
    /// \brief 
    /// Snake animation
    /// \details
    /// Select a color, fastness per led, the amount of snakes.\n
    /// Animation effect: led 1, led 1 and 2, led 1, 2 and 3. etc...\n
    /// Optional to reverse the snake place an extra 1 in the function declaration.
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
    /// \brief 
    /// Flow animation
    /// \details
    /// Create a flow animation by shifting every led to the next led.\n
    /// With ledson and ledsoff the user can decide how many leds will be turned on and off after each other.\n
    /// The fastness is the time between 1 shift. int flows is the amount of flows.
    void flow(const hwlib::color & rgbcolor, int ledson, int ledsoff, int fastness, int flows){
        for(int h = 0; h < flows * (ledcount/(ledson + ledsoff)); h++){
            for(int i = 0; i < (ledsoff+ledson); i++){
                addleds(hwlib::color(0,0,0),i);
                for(int j = 0; j < (ledcount/(ledson + ledsoff)); j++){
                    addleds(rgbcolor, ledson);
                    addleds(hwlib::color(0,0,0), ledsoff);
                }
                hwlib::wait_ms(fastness);
            }
        }
    }

    /// \brief 
    /// Kitt animation
    /// \details
    /// Shows a kitt display. Each led is on for fastness miliseconds. \n
    /// This functions returns when all kits are done.
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
    /// \brief 
    /// disco animation
    /// \details
    /// A fun function that shows different animations and random colors.
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