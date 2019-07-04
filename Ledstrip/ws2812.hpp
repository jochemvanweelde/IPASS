// ==========================================================================
//
// File      : ws2812.hpp
// Part of   : ws2812 ledstrip library
// Copyright : Jochem van Weelde 2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================
#include <hwlib.hpp>
///@file
///@brief

//THIS FUNCTION IS MADE BY WOUTER VAN OOIJEN REPOSITORY WOVO/GODAFOSS
static void __attribute__((noinline)) wait_busy( int32_t n ){
   __asm volatile(                  
      "   .align 4           \t\n"  
      "1: subs.w  r0, #3     \t\n"  
      "   bgt 1b             \t\n"  
      : : "r" ( n )          // uses (reads) n         
   ); 
}

/// \mainpage ws2812b documentation
/// \section Author
/// Jochem van Weelde (First year student Hogeschool Utrecht ICT)\n
/// jochem.vanweelde@student.hu.nl
/// \section Version
/// Version 1.0 (Last modified 4 july 2019)
/// \section Copyright
/// Boost license

/// \brief led_pin
/// led_pin interface (abstract class)
class led_pin{
public:
    /// \brief
    /// Write the led
    /// \details
    /// Write(1) turns the led on\n
    /// Write(0) turns it off.
    virtual void write( bool x ) = 0;

};

/// \brief 
/// ws2812led
/// \details
/// This class stores 1 ws2812led and lets the user control it with multiple functions.\n
/// Example: ws2812led name(due::pin_out(due::pins::##), hwlib::color(###,###,###)); 
class ws2812led : public led_pin{
protected:
    due::pin_out pin;
    hwlib::color rgbcolor;
    hwlib::color off = hwlib::color(0,0,0);
    bool pinoutarray[24];
    bool pinoutarrayoff[24] = {0};
public:
    ws2812led(due::pin_out pin, hwlib::color & rgbcolor):
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
    virtual void write(bool x) override;
    /// \brief 
    /// Change the color of the led.
    /// \details
    /// changecolor(hwlib::color(###,###,###)) changes the color of the led to the color that is given.\n
    /// It's also possible to change the 'off-color' with changecolor(hwlib::color(), 0). \n
    /// If you write a 0 to the led, it will change to that color.
    virtual void changecolor(hwlib::color rgbcolor, bool writebit = 1);
};

/// \brief
/// ws2812led_invert inverts a led
/// \details
/// This class inverts a led. 
/// Write(0) is on, write(1) is off.
/// Handy function in combination with blink!
class ws2812led_invert : public ws2812led{
public:
    ws2812led_invert(due::pin_out pin, hwlib::color rgbcolor):
    ws2812led(pin, rgbcolor){}
    /// \brief
    /// Turn the led on (0) or off(1) (reversed)
    void write(bool x) override;
};

/// \brief
/// ws2812led_dummy does nothing
/// \details
/// Like litterally.... nothing.\n
/// All functions returns nothing and do nothing.
class ws2812led_dummy : public led_pin {
public:

    void write(bool x) override {};

    void changecolor(hwlib::color rgbcolor, bool writebit) {};

    void blink(int ms, int times) {};
};

inline ws2812led_dummy domledje;

/// \brief
/// ws2812led_port add up to 16 leds
/// \details
/// This class lets you add up to 16 leds in one handy class
/// Accepts everything of ws2812led (led_pin)
class ws2812led_port : public led_pin{
private:
    led_pin * leds[16];
    uint_fast8_t ledcount;

public:
    ws2812led_port(
    led_pin & l0 = domledje,
    led_pin & l1 = domledje,
    led_pin & l2 = domledje,
    led_pin & l3 = domledje,
    led_pin & l4 = domledje,
    led_pin & l5 = domledje,
    led_pin & l6 = domledje,
    led_pin & l7 = domledje,
    led_pin & l8 = domledje,
    led_pin & l9 = domledje,
    led_pin & l10 = domledje,
    led_pin & l11 = domledje,
    led_pin & l12 = domledje,
    led_pin & l13 = domledje,
    led_pin & l14 = domledje,
    led_pin & l15 = domledje
    ):
    leds{&l0, &l1, &l2, &l3, &l4, &l5, &l6, &l7, &l8, &l9, &l10, &l11, &l12, &l13, &l14, &l15}
    {
        for( ledcount = 0; ledcount < 16; ++ledcount ){
         if( leds[ ledcount ] == & domledje ){
             break;
         }            
      }
    }

    void write(bool x) override;
};

/// \brief 
/// The led will blink on and off.
/// \details
/// The led writes an 1 and a 0 for a couple of times.\n
/// int ms is the duraion between turning the led on and off.\n
/// int times is how many times the led is going to be turned on.
/// Example: blink(pin, 100, 2); will blink 2 times with 100ms every on/off turn.
/// Accpets every led_pin -> ws2812led.
inline void blink(led_pin & led, int ms, int blinks){
    for(int times = 0; times < blinks; times++){
        led.write(1);
        hwlib::wait_ms(ms);
        led.write(0);
        hwlib::wait_ms(ms);
    }
}

/// \brief 
/// structws2812 (struct)
/// \details
/// This struct is used for ADT ledstrip_array.\n
/// It stores information of 1 led: The 'on color' and the 'off-color'.\n
/// Create a color combination with: structws2812 NAME(hwlib::color(), hwlib::color());\n
/// The standard value is white(on) and black(off).\n
/// For more information, go to ledstrip_array -> changeled() or changegroupled()
struct structws2812{
    hwlib::color on = hwlib::color(255,255,255);
    hwlib::color off = hwlib::color(0,0,0);
    structws2812(hwlib::color on = hwlib::color(255,255,255), hwlib::color off = hwlib::color(0,0,0)):
    on(on),
    off(off){}
};
/// \brief
/// ledstrip_pin interface
class ledstrip_pin{
public:
    /// \brief
    /// Write the led
    virtual void write( bool x) = 0;
    /// \brief
    /// Returns total leds
    virtual int countleds() = 0;
    /// \brief 
    /// Change the color of more leds.
    /// \details
    /// Beginning at 1 and ending at 120 (for a ledstrip with 120 leds).\n
    /// The endled will also be included. Led 0 does nog exist, de ledstrip starts at 1.
    virtual void changegroupled(int beginled, int endled, const structws2812 & rgbstruct) = 0;
};


/// \brief 
/// ledstrip with array (ws2812)
/// \details
/// This class lets the user store a ledstrip and control it.\n
/// This class can store a max of 120 leds on a ledstrip.\n
/// NOTE: The max can be changed by changing the value of the struct length.
class ledstrip_array : public ledstrip_pin{
protected:
    due::pin_out pin;
    int totalleds; 
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
    structws2812 leds[120];
    ledstrip_array(due::pin_out pin, int totalleds):
    pin( pin ),
    totalleds(totalleds)
    {}
    /// \brief 
    /// Write a 1 or a 0 to the whole ledstrip.
    /// \details
    /// write(1) will result in putting every color stored in the structws2812 array on the ledstrip. (Variable on)\n
    /// write(0) will result in putting every color stored in the structws2812 array on the ledstrip. (Variable off)\n
    /// To change the colors go to changeled() for 1 led or changegroupled() for more.
    virtual void write(bool x) override;
    /// \brief 
    /// Change the color of 1 led
    /// \details
    /// Select the led beginning at 1 and select the color combination.\n
    /// Led 0 does nog exist, de ledstrip starts at 1.
    virtual void changeled(int led, const structws2812 & rgbstruct);
    /// \brief 
    /// Change the color of more leds.
    /// \details
    /// Beginning at 1 and ending at 120 (for a ledstrip with 120 leds).\n
    /// The endled will also be included. Led 0 does nog exist, de ledstrip starts at 1.
    virtual void changegroupled(int beginled, int endled, const structws2812 & rgbstruct)override;
    /// \brief 
    /// Shift all colors forward.
    /// \details
    /// Look for an arraw on your ledstrip.\n
    /// This function moves the colors in the direction of the arrows.\n
    /// Colors that leave the ledstrip are coming back on the other side. (circle)\n
    /// shiftforward(3) will shift all the colors 3 leds forward.
    virtual void shiftforward(int times = 1);
    /// \brief 
    /// Shift all colors backwards.
    /// \details
    /// Look for an arraw on your ledstrip.\n
    /// This function moves the colors in the opposite direction of the arrows.\n
    /// Colors that leave the ledstrip are coming back on the other side. (circle)\n
    /// shiftback(3) will shift all the colors 3 leds backward.
    virtual void shiftback(int times = 1);
    /// \brief 
    /// Snake animation
    /// \details
    /// This function adds 1 led at a time\n
    /// For example: snake(50, 2) will result in:\n
    /// 2 snakes and every led will be added  each 50 miliseconds.\n
    /// Animation: led 1, led 1 and 2, led 1 and 2 and 3 etc.\n
    /// The leds wont be turned off at the beginning of this animation. 
    virtual void snake(int fastness, int countsnakes);

    /// \brief
    /// Returns total leds
    virtual int countleds();
};

/// \brief
/// ledstrip_dummy does nothing
/// \details
/// Like litterally.... nothing.\n
/// All functions returns nothing and do nothing.
class ledstrip_dummy : public ledstrip_pin{
public:
    void write(bool x) override{}
    int countleds() override{return 0;}
    virtual void changegroupled(int beginled, int endled, const structws2812 & rgbstruct)override{}
};

inline ledstrip_dummy dumbledstrip;

/// \brief
/// ledstrip_invert inverts the write function
/// \details
/// write(1) turns the led off or the color off.\n
/// write(0) turns the led on.\n
/// Combine this with ledstrip_port for cool effects!
class ledstrip_invert : public ledstrip_array{
public:
    ledstrip_invert(due::pin_out & pin, int totalleds):
    ledstrip_array(pin, totalleds){
    }

    void write(bool x) override;
    void snake(int fastness, int countsnakes) override;
};

/// \brief
/// ledstrip_port for using multiple ledstrips as one.
/// \details
/// Add multiple ledstrip_pin min 1 and max 16 ledstrip_pin.\n
/// This class works with ledstrip_array
class ledstrip_port : public ledstrip_pin{
private:
    ledstrip_pin * ledstrips[16];
    uint_fast8_t ledstripcount;

public:
    ledstrip_port(
    ledstrip_pin & l0 = dumbledstrip,
    ledstrip_pin & l1 = dumbledstrip,
    ledstrip_pin & l2 = dumbledstrip,
    ledstrip_pin & l3 = dumbledstrip,
    ledstrip_pin & l4 = dumbledstrip,
    ledstrip_pin & l5 = dumbledstrip,
    ledstrip_pin & l6 = dumbledstrip,
    ledstrip_pin & l7 = dumbledstrip,
    ledstrip_pin & l8 = dumbledstrip,
    ledstrip_pin & l9 = dumbledstrip,
    ledstrip_pin & l10 = dumbledstrip,
    ledstrip_pin & l11 = dumbledstrip,
    ledstrip_pin & l12 = dumbledstrip,
    ledstrip_pin & l13 = dumbledstrip,
    ledstrip_pin & l14 = dumbledstrip,
    ledstrip_pin & l15 = dumbledstrip
    ):
    ledstrips{&l0, &l1, &l2, &l3, &l4, &l5, &l6, &l7, &l8, &l9, &l10, &l11, &l12, &l13, &l14, &l15}
    {
        for( ledstripcount = 0; ledstripcount < 16; ++ledstripcount ){
         if( ledstrips[ ledstripcount ] == & dumbledstrip ){
             break;
         }            
      }
    }
    /// \brief 
    /// Write a 1 or a 0 to the whole ledstrip_port.
    /// \details
    /// write(1) will result in putting every color stored in the structws2812 array on every ledstrip. (Variable on)\n
    /// write(0) will result in putting every color stored in the structws2812 array on every ledstrip. (Variable off)\n
    /// To change the colors go to changegroupled()
    void write(bool x) override;

    /// \brief
    /// change colors of leds
    /// \details
    /// This function makes it possible to change colors in a port\n
    /// beginled is the first led that is going to change\n
    /// endled is the last led that is going to change.\n
    /// NOTE: The ledcount continues on the next ledstrip.\n
    /// Example (20 led ledstrip, 10 led ledstrip):\n
    /// changegroupled(15,25,green) will change the last 5 leds of the first ledstrip green\n
    /// AND the first 5 leds of the second ledstrip green.\n
    /// This array will also be updated in the pin itself.
    void changegroupled(int beginled, int endled, const structws2812 & rgbstruct) override;


    /// \brief
    /// Returns the total of all the leds in the port
    int countleds() override;
};

/// \brief 
/// The led will blink on and off.
/// \details
/// The led writes an 1 and a 0 for a couple of times.\n
/// int ms is the duraion between turning the led on and off.\n
/// int times is how many times the led is going to be turned on.\n
/// Example: blink(pin , 100, 2); will blink 2 times with 100ms every on/off turn.\n
/// Accpets every ledstrip_pin -> ledstrip_array or port.
inline void blink(ledstrip_pin & pin, int ms, int blinks){
    for(int times = 0; times < blinks; times++){    
        pin.write(1);
        hwlib::wait_ms(ms);
        pin.write(0);
        hwlib::wait_ms(ms);
    }
}


/// \brief 
/// ledstrip ADT for animations
/// \details
/// This ADT lets you control a ledstrip with very easy functions.\n
/// This ADT is focussed on animations. \n
/// If you want to use advanced functions please refer to ledstrip_array or ledstrip_port.\n
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