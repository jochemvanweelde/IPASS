// ==========================================================================
//
// File      : main.cpp
// Copyright : Jochem van Weelde 2019
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// ==========================================================================
#include <hwlib.hpp>
#include "ws2812.hpp"

/*======================================================
This function returns an integer (max 5001)
The integer does not have a unit.
======================================================*/
int getafstand(due::pin_out pinout, due::pin_in pinin){
    pinout.write(1);
    pinout.flush();
    hwlib::wait_us(20);
    pinout.write(0);
    pinout.flush();
    int counter = 0;
    while(pinin.read() == 0){continue;} //Wait until the echo pin turns on (remove delay)
    while(pinin.read()){
        counter++;
        if(counter > 5000){ // max 5001, no unnecessary long wait time
            return counter;
        }
    }
    return counter;
}

/*======================================================
This functions shows all the colors that are true.
The ledstrip will turn off after 400 ms.
======================================================*/
void showcolor(bool bgreen, bool bred, bool byellow, bool bblue, ledstrip_port ledstrippie){
    structws2812 green(hwlib::color(0,255,0));
    structws2812 red(hwlib::color(255,0,0));
    structws2812 yellow(hwlib::color(255,255,0));
    structws2812 blue(hwlib::color(0,0,255));
    structws2812 black(hwlib::color(0,0,0));
    ledstrippie.changegroupled(1,31,black);
    if(bgreen){ledstrippie.changegroupled(9,15,green);}
    if(bred){ledstrippie.changegroupled(16,24,red);}
    if(byellow){ledstrippie.changegroupled(1,8,yellow);}
    if(bblue){ledstrippie.changegroupled(25,31,blue);}
    ledstrippie.write(1);
    hwlib::wait_ms(400);
    ledstrippie.write(0);
}

/*======================================================
This function checks if the correct sensor is activated by the user
It returns true if the corresponding sensor sees the user at a distance lower than 2500
AND the duration is lower than the given duration in the parameter.
It returns false if one or more is is false.
======================================================*/
bool checkcolor(int duration, int color, ledstrip_port ledstrippie, int endwait = 50){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d22 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d12 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d28 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d42 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d24 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d13 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d26 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d44 );

    int distance = 2000;
    int getcolor = -1;
    for(int i = 0; i < duration; i++){
        if(getafstand(sensorGreenOut, sensorGreenIn) < distance){
            showcolor(1, 0, 0, 0, ledstrippie);
            getcolor = 1;
            break;
        }
        else if(getafstand(sensorRedOut, sensorRedIn) < distance){
            showcolor(0, 1, 0, 0, ledstrippie);
            getcolor = 2;
            break;
        }
        else if(getafstand(sensorYellowOut, sensorYellowIn) < distance){
            showcolor(0, 0, 1, 0, ledstrippie);
            getcolor = 3;
            break;
        }
        else if(getafstand(sensorBlueOut, sensorBlueIn) < distance){
            showcolor(0, 0, 0, 1, ledstrippie);
            getcolor = 4;
            break;
        }
        hwlib::wait_ms(endwait);
    }
    return color==getcolor;
}

/*======================================================
This function checks if the correct sensor is activated by the user
It returns true if the corresponding sensor sees the user at a distance lower than 2500
AND the duration is lower than the given duration in the parameter.
It returns false if one or more is is false.
NOTE: this function can check more sensors at once.
That means 2 or more colors at the same time is possible
======================================================*/
bool checkmorecolor(int duration, bool green, bool red, bool yellow, bool blue, ledstrip_port  ledstrippie){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d22 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d12 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d28 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d42 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d24 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d13 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d26 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d44 );

    int distance = 2000;
    bool checkgreen = 0, checkred = 0, checkyellow = 0, checkblue = 0;
    structws2812 colorgreen(hwlib::color(0,255,0));
    structws2812 colorred(hwlib::color(255,0,0));
    structws2812 coloryellow(hwlib::color(255,255,0));
    structws2812 colorblue(hwlib::color(0,0,255));
    structws2812 colorblack(hwlib::color(0,0,0));
    ledstrippie.changegroupled(1,31,colorblack);
    ledstrippie.write(1);

    for(int i = 0; i < duration; i++){
        if(getafstand(sensorGreenOut, sensorGreenIn) < distance){
            if(green == 0){return 0;}
            checkgreen = green;
            ledstrippie.changegroupled(9,15,colorgreen);
        }
        if(getafstand(sensorRedOut, sensorRedIn) < distance){
            if(red == 0){return 0;}
            checkred = red;
            ledstrippie.changegroupled(16,24,colorred);
        }
        if(getafstand(sensorYellowOut, sensorYellowIn) < distance){
            if(yellow == 0){return 0;}
            checkyellow = yellow;
            ledstrippie.changegroupled(1,8,coloryellow);
        }
        if(getafstand(sensorBlueOut, sensorBlueIn) < distance){
            if(blue == 0){return 0;}
            checkblue = blue;
            ledstrippie.changegroupled(25,31,colorblue);
        }
        hwlib::wait_ms(50);
        ledstrippie.write(1);
        if(checkgreen == green && checkred == red && checkyellow == yellow && checkblue == blue){
            return 1;
        }
    }
    return 0;
}

/*======================================================
Can we get an F in the chat....
======================================================*/
bool getcolor(int duration, bool green, bool red, bool yellow, bool blue, ledstrip_port ledstrippin){
    for(int i = 0; i < duration; i++){
        if(green && checkcolor(1, 1, ledstrippin, 0)){green = false;}
        if(red   && checkcolor(1, 2, ledstrippin, 0)){red   = false;}
        if(yellow&& checkcolor(1, 3, ledstrippin, 0)){yellow= false;}
        if(blue  && checkcolor(1, 4, ledstrippin, 0)){blue  = false;}
    }
    return !(green || red || yellow || blue);
}
/*======================================================
This function can make different notes with the buzzer
The delayf changes the note and loops changes the duration.
If you want notes of the same length (delayf * loops) must be the same every time
======================================================*/
void buzzer(int delayf, int loops){
    due::pin_out buzzerpin = due::pin_out( due::pins::d52 );
    for (int i = 0; i < loops; i++) {
      buzzerpin.write(1);
      hwlib::wait_us(delayf);
      buzzerpin.write(0);
      hwlib::wait_us(delayf);
    }
}
/*======================================================
Normal simon is the simon everyone knows and loves
It begins with 1 color and every turn 1 color is added to the list
Normally the game never ends. Arduino Due doest have a stack,
so the array length is set to 100, try to beat that ;).
======================================================*/
void normalsimon(ledstrip_port ledstrippin){
    //ledstrip
    int array[100] = {0};
    for(int i = 0; i < 100; i++){ //Make random array before we start the game
        array[i] = rand() % 4 + 1;
    }
    bool gaatgoed = true;
    int beurt = 0;
    while(gaatgoed){
        for(int i = 0; i < beurt; i++){ //Show colors every turn shows one more color
            showcolor(array[i] == 1, array[i] == 2, array[i] == 3, array[i] == 4, ledstrippin);
            hwlib::wait_ms(400);
        }
        buzzer(800, 50);
        hwlib::wait_ms(100);
        buzzer(1000, 150); //buzzer effect so the user knows when he can play
        for(int i = 0; i < beurt; i++){
            if(!gaatgoed){break;} //If wrong sensor -> break
            gaatgoed = checkcolor(20, array[i], ledstrippin); //Check if the user activates the right colors
        }
        hwlib::wait_ms(300);
        if(beurt >= 100 || !gaatgoed){ //This is made so a user cant go out of bounce
            break; //Stop program
        }else{
            beurt++;
        }

    }
    //ledstrippin.changegroupled(1,31,hwlib::color(255,0,0));
    //ledstrippin.snake(40, 2); //Show a red snake, the user now knows it's game over.
    return;
}

/*======================================================
This function return a 1 or a 0.
It's random...
======================================================*/
bool randbool(){
    return rand() % 2;
}

/*======================================================
This is a weird variation of simon says. 
It is played with different levels.
Every level gets harder and harder. 
In this mode sometimes 2 colors will light up
The user will have to use both hands to complete it.
======================================================*/
void levels(ledstrip_port ledstrippie){
    int doublecolor[24] = {1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,0,1,0,1}; //All(6) possible double colors. Every four integers represents 1 possible outcome
    int colorarray[15]= {0};
    //Levels: 1= 1 random color; 2= 2 random colors at the same time (I didn't have more time to make 3 (slide) and 4. I'll make them in my own time ;) 
    int level1[] = {1,1,2};
    int level2[] = {1,2,1,2};
    int level3[] = {2,2,2,2,1};
    int level4[] = {1,2,1,2,1,2};
    int *levels[] = {level1, level2, level3, level4}; //Pointer because it's an array with array's with variable lengths.
    int sizeoflevels[4] = {3,4,5,6}; // Keep track of the size of the levels.
    bool gaatgoed = true;
    int level = 0;
    int aantallevels = 4; // Keep track of the total level that exist.
    int beurt;    
    for(;;){
        //Add one random color to the colorarray every turn.
        for(int j = 0; j < aantallevels; j++){ //Loop for every level
            beurt = 1;
            for(int k = 0; k < sizeoflevels[j]; k++){ //Loop for every move in the level
                if(levels[j][beurt-1] == 1){ //Make the color array (1 color at a time)
                    colorarray[beurt-1] = rand() % 4 + 1;
                }else if(levels[j][beurt-1] == 2){ //Make a color array(2 colors at a time)
                    colorarray[beurt-1] =rand() % 6 + 5;
                }
                for(int i = 0; i < beurt; i++){ //Show all the colors in the array
                    if(colorarray[i] <= 4){ //colorarray 1-4
                        showcolor(colorarray[i] == 1, colorarray[i] == 2, colorarray[i] == 3, colorarray[i] == 4, ledstrippie);
                        hwlib::wait_ms(400);
                    }else if(colorarray[i] <= 10){ //colorarray 5-10
                        int colord = (colorarray[i]-5)*4;
                        showcolor(doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippie); //Selects a situation in the pre-created array
                        hwlib::wait_ms(400);
                    }
                }
                buzzer(800, 50);
                hwlib::wait_ms(100);
                buzzer(1000, 150); //Buzzer tone so the user knows when he can play
                for(int i = 0; i < beurt; i++){ //Let the user reapeat the sequence and check if the user did it correctly.
                    if(colorarray[i] <= 4){
                        gaatgoed = checkcolor(20, colorarray[i], ledstrippie);
                        hwlib::wait_ms(300);
                        if(!gaatgoed){ //WRONG! input by user.
                            for(int a = 0; a < 3; a++){
                                ledstrippie.changegroupled(1,31,hwlib::color(255,0,0));
                                ledstrippie.write(1);
                                buzzer(1000, 300);
                                showcolor(colorarray[i] == 1, colorarray[i] == 2, colorarray[i] == 3, colorarray[i] == 4, ledstrippie); //Show the correct color
                                buzzer(500, 600);
                            }
                            return;
                        }
                    }else if(colorarray[i] <= 10){
                        int colord = (colorarray[i]-5)*4;
                        gaatgoed = checkmorecolor(20, doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippie);
                        hwlib::wait_ms(300);
                        ledstrippie.write(0);
                        if(!gaatgoed){ //WRONG! input by user.
                            for(int a = 0; a < 3; a++){
                                ledstrippie.changegroupled(1,31,hwlib::color(255,0,0));
                                ledstrippie.write(1);
                                buzzer(1000, 300);
                                showcolor(doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippie); //Show the correct colors
                                buzzer(500, 600);
                            }
                            return;
                        }
                    }
                }
                beurt++;
                ledstrippie.write(0);
                hwlib::wait_ms(500);
            }
            ledstrippie.changegroupled(1,31,hwlib::color(0,255,0));
            blink(ledstrippie, 200, 4);
            hwlib::wait_ms(1500);
            level++;
        }
    }
}

/*======================================================
The main function.
The program will start and cycles through 3 colors.
RED = LEVELS 1 player.
GREEN = NORMAL SIMON
YELLOW = BREAK PROGRAM(STOP) //2 player simon -> no time
======================================================*/
int main(void){
    hwlib::wait_ms(1000);
    due::pin_out ledstrippin1 = due::pin_out( due::pins::d53 );
    due::pin_out ledstrippin2 = due::pin_out( due::pins::d8 );
    ledstrip ledbegin(ledstrippin1, 24);
    ledstrip_array ledstrip1(ledstrippin1, 24);
    ledstrip_array ledstrip2(ledstrippin2, 7);
    ledstrip_port ledstrip3(ledstrip1, ledstrip2);
    int duration = 30;
    int ledcount = 24;
    uint32_t srandinteger = 2147483647;
    while(!(checkcolor(1, 4, ledstrip3))){
        srandinteger--;
    }
    hwlib::wait_ms(400);
    srand(srandinteger);
    for(;;){        
        ledbegin.fillleds(hwlib::color(0,255,0), ledcount);
        if(checkcolor(duration, 4, ledstrip3)){
            ledbegin.fillleds(hwlib::color(0,0,0), ledcount);
            hwlib::wait_ms(2000);
            normalsimon(ledstrip3);
        }
        ledbegin.fillleds(hwlib::color(255, 0 ,0), ledcount);
        if(checkcolor(duration, 4, ledstrip3)){
            ledbegin.fillleds(hwlib::color(0,0,0), ledcount);
            hwlib::wait_ms(2000);
            levels(ledstrip3);
            ledbegin.fillleds(hwlib::color(255,255,255), ledcount);
            hwlib::wait_ms(2000);        
        }
        ledbegin.fillleds(hwlib::color(255,255,0), ledcount);
        if(checkcolor(duration, 4, ledstrip3)){break;}
    }
    return 0;
}