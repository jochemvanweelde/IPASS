#include <hwlib.hpp>
#include "simonlib.hpp"



void showcolor(bool bgreen, bool bred, bool byellow, bool bblue, due::pin_out ledstrippin){
    structw2812 green(hwlib::color(0,255,0));
    structw2812 red(hwlib::color(255,0,0));
    structw2812 yellow(hwlib::color(255,255,0));
    structw2812 blue(hwlib::color(0,0,255));
    structw2812 black(hwlib::color(0,0,0));
    ledstrip_array ledstripje(ledstrippin, 60);
    ledstripje.changegroupled(1,60,black);
    if(bgreen){ledstripje.changegroupled(1,15,green);}
    if(bred){ledstripje.changegroupled(16,30,red);}
    if(byellow){ledstripje.changegroupled(31,45,yellow);}
    if(bblue){ledstripje.changegroupled(46,60,blue);}
    ledstripje.write(1);
    hwlib::wait_ms(400);
    ledstripje.write(0);
}

//duration in miliseconds
bool checkcolor(int duration, int color, due::pin_out ledstrippin, int endwait = 50){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d3 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d11 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d8 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d4 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d2 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d10 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d9 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d5 );

    int distance = 1000;
    int getcolor = -1;
    for(int i = 0; i < duration; i++){
        if(getafstand(sensorGreenOut, sensorGreenIn) < distance){
            showcolor(1, 0, 0, 0, ledstrippin);
            //hwlib::cout << "Goed! groen\n";
            getcolor = 1;
            break;
        }
        else if(getafstand(sensorRedOut, sensorRedIn) < distance){
            showcolor(0, 1, 0, 0, ledstrippin);
            //hwlib::cout << "Goed! rood\n";
            getcolor = 2;
            break;
        }
        else if(getafstand(sensorYellowOut, sensorYellowIn) < distance){
            showcolor(0, 0, 1, 0, ledstrippin);
            //hwlib::cout << "Goed! geel\n";
            getcolor = 3;
            break;
        }
        else if(getafstand(sensorBlueOut, sensorBlueIn) < distance){
            showcolor(0, 0, 0, 1, ledstrippin);
            //hwlib::cout << "Goed! blauw\n";
            getcolor = 4;
            break;
        }
        hwlib::wait_ms(endwait);
    }
    //hwlib::cout << color << "   " << getcolor << "\n";
    return color==getcolor;
}

bool checkmorecolor(int duration, bool green, bool red, bool yellow, bool blue, due::pin_out  ledstrippin){
    due::pin_out sensorGreenOut = due::pin_out( due::pins::d3 );
    due::pin_out sensorRedOut = due::pin_out( due::pins::d11 );
    due::pin_out sensorYellowOut = due::pin_out( due::pins::d8 );
    due::pin_out sensorBlueOut = due::pin_out( due::pins::d4 );

    due::pin_in sensorGreenIn = due::pin_in( due::pins::d2 );
    due::pin_in sensorRedIn = due::pin_in( due::pins::d10 );
    due::pin_in sensorYellowIn = due::pin_in( due::pins::d9 );
    due::pin_in sensorBlueIn = due::pin_in( due::pins::d5 );

    int distance = 500;
    bool checkgreen = 0, checkred = 0, checkyellow = 0, checkblue = 0;
    structw2812 colorgreen(hwlib::color(0,255,0));
    structw2812 colorred(hwlib::color(255,0,0));
    structw2812 coloryellow(hwlib::color(255,255,0));
    structw2812 colorblue(hwlib::color(0,0,255));
    structw2812 colorblack(hwlib::color(0,0,0));
    ledstrip_array ledstripje(ledstrippin, 60);
    ledstripje.changegroupled(1,60,colorblack);
    ledstripje.write(1);

    for(int i = 0; i < duration; i++){
        if(getafstand(sensorGreenOut, sensorGreenIn) < distance){
            if(green == 0){return 0;}
            checkgreen = green;
            ledstripje.changegroupled(1,15,colorgreen);
        }
        if(getafstand(sensorRedOut, sensorRedIn) < distance){
            if(red == 0){return 0;}
            checkred = red;
            ledstripje.changegroupled(16,30,colorred);
        }
        if(getafstand(sensorYellowOut, sensorYellowIn) < distance){
            if(yellow == 0){return 0;}
            checkyellow = yellow;
            ledstripje.changegroupled(31,45,coloryellow);
        }
        if(getafstand(sensorBlueOut, sensorBlueIn) < distance){
            if(blue == 0){return 0;}
            checkblue = blue;
            ledstripje.changegroupled(46,60,colorblue);
        }
        hwlib::wait_ms(50);
        ledstripje.write(1);
        if(checkgreen == green && checkred == red && checkyellow == yellow && checkblue == blue){
            return 1;
        }
    }
    return 0;
}

//approx 200 ms per loop
bool getcolor(int duration, bool green, bool red, bool yellow, bool blue, due::pin_out ledstrippin){
    for(int i = 0; i < duration; i++){
        if(green && checkcolor(1, 1, ledstrippin, 0)){green = false;}
        if(red   && checkcolor(1, 2, ledstrippin, 0)){red   = false;}
        if(yellow&& checkcolor(1, 3, ledstrippin, 0)){yellow= false;}
        if(blue  && checkcolor(1, 4, ledstrippin, 0)){blue  = false;}
    }
    return !(green || red || yellow || blue);
}

void normalsimon(due::pin_out ledstrippin, ledstrip ledstrippie){
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
        if(beurt >= 15 || !gaatgoed){
            
            gaatgoed = false;
        }else{
            //ledstrippie.snake(hwlib::color(0,255,0), 40, 1);
            beurt++;
        }

    }
    ledstrippie.snake(hwlib::color(255,0,0), 40, 2);
    hwlib::cout << "Het einde van het hele programma\n";
    return;
}

bool randbool(){
    return rand() % 2;
}

void levels(due::pin_out ledstrippin, ledstrip ledstrippie){
    int doublecolor[24] = {1,1,0,0,0,1,1,0,0,0,1,1,1,0,0,1,1,0,1,0,0,1,0,1};
    int colorarray[15]= {0};
    int level1[] = {1,2,2};
    int level2[] = {2,1,2,1};
    int level3[] = {2,2,2,2,1};
    int *levels[] = {level1, level2, level3};
    int sizeoflevels[3] = {3,4,5};
    bool gaatgoed = true;
    int level = 0;
    for(;;){
        int beurt = 1;
        for(int j = 0; j < sizeoflevels[level]; j++){
            hwlib::cout << beurt << " ste/de beurt\n";
            for(int i = beurt-1; i < beurt; i++){
                if(levels[j][i] == 1){
                    colorarray[i] = rand() % 4 + 1;
                    hwlib::cout << colorarray[i] << "\n";
                }else if(levels[j][i] == 2){
                    colorarray[i] =rand() % 4 + 5;
                    hwlib::cout << colorarray[i] << "\n";
                }
            }
            hwlib::cout << "===================" << "\n";
            for(int i = 0; i < beurt; i++){
                if(colorarray[i] <= 4){
                    showcolor(colorarray[i] == 1, colorarray[i] == 2, colorarray[i] == 3, colorarray[i] == 4, ledstrippin);
                    hwlib::wait_ms(400);
                }else if(colorarray[i] <= 8){
                    int colord = (colorarray[i]-5)*4;
                    hwlib::cout << colord << " colord \n";
                    showcolor(doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippin);
                    hwlib::wait_ms(400);
                }
            }
            hwlib::cout << "===================" << "\n";
            for(int i = 0; i < beurt; i++){
                if(colorarray[i] <= 4){
                    gaatgoed = checkcolor(20, colorarray[i], ledstrippin);
                    hwlib::wait_ms(400);
                }else if(colorarray[i] <= 8){
                    int colord = (colorarray[i]-5)*4;
                    gaatgoed = checkmorecolor(60, doublecolor[colord], doublecolor[colord+1], doublecolor[colord+2], doublecolor[colord+3], ledstrippin);
                    hwlib::wait_ms(400);
                }
                if(!gaatgoed){
                    hwlib::cout << "FOUT GEMAAKT\n";
                    ledstrippie.snake(hwlib::color(255,0,0), 40, 2);
                    return;
                }
            }
            hwlib::cout << "//////////////////" << "\n";
            beurt++;
        }
        ledstrippie.snake(hwlib::color(0,255,0), 40, 1);
        level++;
    }
}

int main(void){
    hwlib::wait_ms(1000);
    srand(987);
    due::pin_out ledstrippin = due::pin_out( due::pins::d7 );
    ledstrip ledstrippie(ledstrippin, 60);
    int duration = 30;
    for(;;){        
        ledstrippie.fillleds(hwlib::color(0,255,0), 60);
        if(checkcolor(duration, 4, ledstrippin)){
            ledstrippie.fillleds(hwlib::color(0,0,0), 60);
            hwlib::wait_ms(2000);
            normalsimon(ledstrippin, ledstrippie);
        }
        ledstrippie.fillleds(hwlib::color(255, 0 ,0), 60);
        if(checkcolor(duration, 4, ledstrippin)){
            ledstrippie.fillleds(hwlib::color(0,0,0), 60);
            hwlib::wait_ms(2000);
            levels(ledstrippin, ledstrippie);
            ledstrippie.fillleds(hwlib::color(255,255,255), 60);
            hwlib::wait_ms(2000);        
        }
        ledstrippie.fillleds(hwlib::color(255,255,0), 60);
        if(checkcolor(duration, 4, ledstrippin)){break;}
    }
    return 0;
}