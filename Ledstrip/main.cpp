#include <hwlib.hpp>
#include "simonlib.hpp"

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

void showcolor(bool bgreen, bool bred, bool byellow, bool bblue, ledstripport ledstrippie){
    structw2812 green(hwlib::color(0,255,0));
    structw2812 red(hwlib::color(255,0,0));
    structw2812 yellow(hwlib::color(255,255,0));
    structw2812 blue(hwlib::color(0,0,255));
    structw2812 black(hwlib::color(0,0,0));
    ledstrippie.changegroupled(1,31,black);
    if(bgreen){ledstrippie.changegroupled(9,15,green);}
    if(bred){ledstrippie.changegroupled(16,24,red);}
    if(byellow){ledstrippie.changegroupled(1,8,yellow);}
    if(bblue){ledstrippie.changegroupled(25,31,blue);}
    ledstrippie.write(1);
    hwlib::wait_ms(400);
    ledstrippie.write(0);
}

//duration in miliseconds
bool checkcolor(int duration, int color, ledstripport ledstrippie, int endwait = 50){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d22 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d12 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d28 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d42 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d24 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d13 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d26 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d44 );

    int distance = 1500;
    int getcolor = -1;
    for(int i = 0; i < duration; i++){
        if(getafstand(sensorGreenOut, sensorGreenIn) < distance){
            showcolor(1, 0, 0, 0, ledstrippie);
            //hwlib::cout << "Goed! groen\n";
            getcolor = 1;
            break;
        }
        else if(getafstand(sensorRedOut, sensorRedIn) < distance){
            showcolor(0, 1, 0, 0, ledstrippie);
            //hwlib::cout << "Goed! rood\n";
            getcolor = 2;
            break;
        }
        else if(getafstand(sensorYellowOut, sensorYellowIn) < distance){
            showcolor(0, 0, 1, 0, ledstrippie);
            //hwlib::cout << "Goed! geel\n";
            getcolor = 3;
            break;
        }
        else if(getafstand(sensorBlueOut, sensorBlueIn) < distance){
            showcolor(0, 0, 0, 1, ledstrippie);
            //hwlib::cout << "Goed! blauw\n";
            getcolor = 4;
            break;
        }
        hwlib::wait_ms(endwait);
    }
    //hwlib::cout << color << "   " << getcolor << "\n";
    return color==getcolor;
}

bool checkmorecolor(int duration, bool green, bool red, bool yellow, bool blue, ledstripport  ledstrippie){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d22 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d12 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d28 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d42 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d24 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d13 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d26 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d44 );

    int distance = 2500;
    bool checkgreen = 0, checkred = 0, checkyellow = 0, checkblue = 0;
    structw2812 colorgreen(hwlib::color(0,255,0));
    structw2812 colorred(hwlib::color(255,0,0));
    structw2812 coloryellow(hwlib::color(255,255,0));
    structw2812 colorblue(hwlib::color(0,0,255));
    structw2812 colorblack(hwlib::color(0,0,0));
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

//approx 200 ms per loop
bool getcolor(int duration, bool green, bool red, bool yellow, bool blue, ledstripport ledstrippin){
    for(int i = 0; i < duration; i++){
        if(green && checkcolor(1, 1, ledstrippin, 0)){green = false;}
        if(red   && checkcolor(1, 2, ledstrippin, 0)){red   = false;}
        if(yellow&& checkcolor(1, 3, ledstrippin, 0)){yellow= false;}
        if(blue  && checkcolor(1, 4, ledstrippin, 0)){blue  = false;}
    }
    return !(green || red || yellow || blue);
}

void normalsimon(ledstripport ledstrippin){
    //ledstrip
    int array[20] = {0};
    for(int i = 0; i < 20; i++){
        array[i] = rand() % 4 + 1;
    }
    bool gaatgoed = true;
    int beurt = 0;
    while(gaatgoed){
        for(int i = 0; i < beurt; i++){
            showcolor(array[i] == 1, array[i] == 2, array[i] == 3, array[i] == 4, ledstrippin);
            hwlib::wait_ms(400);
        }
        for(int i = 0; i < beurt; i++){
            if(!gaatgoed){break;}
            gaatgoed = checkcolor(10, array[i], ledstrippin);
        }
        hwlib::wait_ms(300);
        if(beurt >= 19 || !gaatgoed){
            
            gaatgoed = false;
        }else{
            //ledstrippie.snake(hwlib::color(0,255,0), 40, 1);
            beurt++;
        }

    }
    ledstrippin.changegroupled(1,31,hwlib::color(255,0,0));
    ledstrippin.snake(40, 2);
    hwlib::cout << "Het einde van het hele programma\n";
    return;
}

bool randbool(){
    return rand() % 2;
}

void levels(ledstripport ledstrippie){
    int doublecolor[24] = {1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,0,1,0,1};
    int colorarray[15]= {0};
    int level1[] = {1,1,2};
    int level2[] = {1,2,1,2};
    int level3[] = {2,2,2,2,1};
    int *levels[] = {level1, level2, level3};
    int sizeoflevels[3] = {3,4,5};
    bool gaatgoed = true;
    int level = 0;
    int aantallevels = 3;
    for(;;){
        for(int j = 0; j < aantallevels-1; j++){
            int beurt = 1;
            for(int k = 0; k < sizeoflevels[j]; k++){
                hwlib::cout << beurt << " ste/de beurt\n";
                for(int i = beurt-1; i < beurt; i++){
                    if(levels[j][i] == 1){
                        colorarray[i] = rand() % 4 + 1;
                    }else if(levels[j][i] == 2){
                        colorarray[i] =rand() % 4 + 5;
                    }
                    hwlib::cout << colorarray[i] << "\n" << levels[j][i] << "\n";
                }
                hwlib::cout << "===================" << "\n";
                for(int i = 0; i < beurt; i++){
                    if(colorarray[i] <= 4){
                        showcolor(colorarray[i] == 1, colorarray[i] == 2, colorarray[i] == 3, colorarray[i] == 4, ledstrippie);
                        hwlib::wait_ms(400);
                    }else if(colorarray[i] <= 8){
                        int colord = (colorarray[i]-5)*4;
                        hwlib::cout << colord << " colord \n";
                        showcolor(doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippie);
                        hwlib::wait_ms(400);
                    }
                }
                hwlib::cout << "===================" << "\n";
                for(int i = 0; i < beurt; i++){
                    if(colorarray[i] <= 4){
                        gaatgoed = checkcolor(20, colorarray[i], ledstrippie);
                        hwlib::wait_ms(400);
                    }else if(colorarray[i] <= 8){
                        int colord = (colorarray[i]-5)*4;
                        gaatgoed = checkmorecolor(60, doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippie);
                        hwlib::wait_ms(400);
                    }
                    if(!gaatgoed){
                        hwlib::cout << "FOUT GEMAAKT\n";
                        ledstrippie.changegroupled(1,31,hwlib::color(255,0,0));
                        ledstrippie.snake(40, 2);
                        return;
                    }
                }
                hwlib::cout << "//////////////////" << "\n";
                beurt++;
                hwlib::wait_ms(500);
            }
            ledstrippie.changegroupled(1,31,hwlib::color(0,255,0));
            ledstrippie.snake(40, 1);
            hwlib::wait_ms(1000);
            level++;
        }
    }
}

int main(void){
    hwlib::wait_ms(1000);
    srand(987);
    due::pin_out ledstrippin1 = due::pin_out( due::pins::d53 );
    due::pin_out ledstrippin2 = due::pin_out( due::pins::d8 );
    ledstrip ledbegin(ledstrippin1, 24);
    ledstrip_array ledstrip1(ledstrippin1, 24);
    ledstrip_invert ledstrip2(ledstrippin2, 7);
    ledstripport ledstrip3(ledstrip1, ledstrip2);
    // int duration = 30;
    // int ledcount = 24;
    // for(;;){        
    //     ledbegin.fillleds(hwlib::color(0,255,0), ledcount);
    //     if(checkcolor(duration, 4, ledstrip3)){
    //         ledbegin.fillleds(hwlib::color(0,0,0), ledcount);
    //         hwlib::wait_ms(2000);
    //         normalsimon(ledstrip3);
    //     }
    //     ledbegin.fillleds(hwlib::color(255, 0 ,0), ledcount);
    //     if(checkcolor(duration, 4, ledstrip3)){
    //         ledbegin.fillleds(hwlib::color(0,0,0), ledcount);
    //         hwlib::wait_ms(2000);
    //         levels(ledstrip3);
    //         ledbegin.fillleds(hwlib::color(255,255,255), ledcount);
    //         hwlib::wait_ms(2000);        
    //     }
    //     ledbegin.fillleds(hwlib::color(255,255,0), ledcount);
    //     if(checkcolor(duration, 4, ledstrip3)){break;}
    // }

    ledstrip3.snake(50, 20);

    return 0;
}