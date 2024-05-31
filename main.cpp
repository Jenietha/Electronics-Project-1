/*Program to display simple messages and values on an LCD screen
Adapted from publically available GITHUB program for an arduino
01/11/23
Version 3
*/

/* Ensure RW LCD pin is connected to GND and connect a 10 K potentiometer with wiper connected to V0 and other ends to A and K LCD pins.
 Connect a 100 ohm resistor (ideally 500 Ohm) between A LCD pin and VDD.
 */

#include "mbed.h"
#include "TextLCD.h"
#include "string.h"
#include <string>


DigitalIn buttonright(D8, PullUp);
DigitalIn buttonleft(D10, PullUp);
DigitalIn button3(D7, PullUp);
InterruptIn buttonInventory(D12, PullUp);

Semaphore button3Semaphore(0,1);

Thread button3Thread;

TextLCD lcd(D0, D1, D2, D3, D4, D5, TextLCD::LCD16x2); // Connect these nucleo pins to RS, E, D4, D5, D6 and D7 pins of the LCD

int button1_right = 1;
int button2_left = 0;
int button3press = 0;

int customcharStartAddress = 0x40;
int customcharAddressIncrement = 8;

int scrolltime = 500;
int lcdcooldowntime = 50;
int blinkonoff = 500;


int exploreoptions[4] = {0x46, 0x42 ,0x4C, 0x52}; //actions during an exploration scene
int battleoptions[4] = {1,2,3,0x25}; //actions during a battle scene

/*string knife = "Knife";
string lizarddagger = "Lizard Dagger";
string lizardknife = "Lizard Knife";
string stick = "Wooden Stick";
string wolfpaw = "Wolf Paw";
string wolfskin = "Wolf Skin";
string skeletonblade = "Skeleton Blade";
string hammer = "Hammer";
string stonegloves = "Stone Gloves";
string stonearmour = "Stone Armour";
string flamingsword = "Flaming Sword";
string breakerhammer = "Breaker Hammer";
string armouredlizardskin = "Ar.Lizard Skin";
string flowingarmour = "Flowing Armour";
string woodenboard = "Wooden Board";
string greencrystal = "Green Crystal";
string redcrystal = "Red Crystal";
string bluecrystal = "Blue Crystal";
*/

char const * knifepointer = "Knife";
char const * lizarddaggerpointer = "Lizard Dagger";
char const * lizardknifepointer = "Lizard Knife";
char const * stickpointer = "Stick";

char const * inventory[] = {knifepointer, lizarddaggerpointer, lizardknifepointer, stickpointer};


int healthvalue=5; //initial value for player's health
int speedvalue=6; //initial value for player's speed
int defencevalue=4; //initial value for player's defensive strength
int powervalue=4; //initial value for player's offensive strength

int healthmaxvalue = 40;
int speedmaxvalue = 40;
int defencemaxvalue = 40;
int powermaxvalue = 40;


int button1right_counter(){
    if(buttonright == false){
        ++button1_right;
    }
    return button1_right;
}

int button2left_counter(){
    if(buttonleft == false){
        ++button2_left;    
    }
    return button2_left;
}

int button3_counter(){
    if(button3 == false){
        if(button3press >3 ){
            button3press = 0;
        }
        else{
            ++button3press;
        }
    }
    return button3press;
}

//character for player's health
char healthicon[8] ={ 0x00, 0x0A, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00};

//character for player's health value
char healthlevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//character for player's speed and action icon for run/dodge
char speedicon[8] = { 0x00, 0x07, 0x0F, 0x1E, 0x1C, 0x0E, 0x00,0x00};

//character for player's speed value
char speedlevel[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//character for player's defence and action icon for defending
char defenceicon[8] = { 0x00, 0x1F, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00};

//character for player's defensive strength value
char defencelevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//character for player's power and action icon for attack
char powericon[8] = { 0x04, 0x0E, 0x0E, 0x0E, 0x0E, 0x1F, 0x04, 0x04};

//character for player's power value
char powerlevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

//Functions to scroll text
void showletters(int printStart, int startLetter, char const *text){
    lcd.locate(printStart,0);
    for(int letter = startLetter; letter<=startLetter+15; ++letter){
        lcd.printf("%c", text[letter]);
    }
    lcd.printf(" ");
    thread_sleep_for(lcdcooldowntime);
}

void Scroll(char const *text){
    for (int letter = 0; letter<= strlen(text)-16; ++letter){
        showletters(0,letter, text);
        thread_sleep_for(scrolltime);
    }
}

//Function to blink characters - to show they are selected
void blinktext(int option1, int option2, int option3){

    if(abs(button1_right-button2_left)%4 == 0 ){
        lcd.locate(6,1);
        lcd.printf("i");
        thread_sleep_for(blinkonoff);
        lcd.locate(6,1);
        lcd.printf(" ");
        thread_sleep_for(blinkonoff);
    }

    else if (abs(button1_right-button2_left) % 3 == 0 && abs(button1_right-button2_left) % 2 != 0){
        lcd.locate(4,1);
        lcd.putc(option3);
        thread_sleep_for(blinkonoff);
        lcd.locate(4,1);
        lcd.printf(" ");
        thread_sleep_for(blinkonoff);
    }
    else{
        if(abs(button1_right-button2_left) % 2 == 0){
            lcd.locate(2,1);
            lcd.putc(option2);
            thread_sleep_for(blinkonoff);
            lcd.locate(2,1);
            lcd.printf(" ");
            thread_sleep_for(blinkonoff);
        }
        else{
            lcd.locate(0,1);
            lcd.putc(option1);
            thread_sleep_for(blinkonoff);
            lcd.locate(0,1);
            lcd.printf(" ");
            thread_sleep_for(blinkonoff);
        }
    }
}

void displaystats(){
    lcd.locate(8,1);
    lcd.putc(0);

    lcd.locate(9,1);
    lcd.putc(1);

    lcd.locate(10,1);
    lcd.putc(2);

    lcd.locate(11,1);
    lcd.putc(3);

    lcd.locate(12,1);
    lcd.putc(4);

    lcd.locate(13,1);
    lcd.putc(5);

    lcd.locate(14,1);
    lcd.putc(6);

    lcd.locate(15,1);
    lcd.putc(7);

}


void option1(){
    Scroll("You enter the forest.");
    displaystats();



    sleep();

}

void option2(){
    Scroll("You win the fight.");
    thread_sleep_for(2000);
    Scroll("You obtain a knife.");

    displaystats();

    sleep();

}

void option3(){
    Scroll("You are caught up by the goblin.")
    displaystats();
    sleep();

}

void showInventory(){
    //if (abs(button1_right-button2_left) % 4 == 0 && button3 == false){

        lcd.cls();
        lcd.locate(15,0);
        lcd.putc(0x7F);

         

        for (int button3press = 0; button3press < 4; ++button3press){

            button3_counter();

            if(button3press == 3){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("%s",inventory[button3press]);
            }
            else{
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("%s",inventory[button3press]);

                lcd.locate(0,1);
                lcd.printf("%s",inventory[button3press+1]);

            }
            
            thread_sleep_for(2000);
        }



        /*lcd.locate(0,0);
        lcd.printf("%s",inventory[0]);

        lcd.locate(0,1);
        lcd.printf("%s",inventory[1]);*/

        /*button3_counter();

        while(true){
            if (button3press >= 3){
                button3press = 0;
            }

            if(button3 == true){
                //lcd.cls();
                lcd.locate(0,0);
                lcd.printf("%s",inventory[button3press]);

                lcd.locate(0,1);
                lcd.printf("%s",inventory[button3press++]);

            }

            

        }*/

        

    } 
//}

void button3Fn(){
    button3Semaphore.release();
}



void mainscreen(){

    lcd.locate(0,0);
    Scroll("You enter the plains.");
    thread_sleep_for(3000);

    Scroll("You encounter a goblin.");

    displaystats();


    lcd.locate(0,1);
    lcd.putc(battleoptions[0]);

    lcd.locate(2,1);
    lcd.putc(battleoptions[1]);

    lcd.locate(4,1);
    lcd.putc(battleoptions[3]);

    lcd.locate(6,1);
    lcd.printf("i");

    blinktext(battleoptions[0], battleoptions[1], battleoptions[3]);
    
}


int main() 
{
    //Health icon
    lcd.writeCommand(customcharStartAddress);

    for(int i = 0;i < 8;i++){
        lcd.writeData(healthicon[i]);
    }


    //health data
    lcd.writeCommand(customcharStartAddress + customcharAddressIncrement);

    for (int u = 0; u < 8; ++u){
        healthlevel[u]=0x00;
        lcd.writeData(healthlevel[u]);
    }

    lcd.writeCommand(customcharStartAddress + customcharAddressIncrement);
    for (int u=0; u<healthvalue+1 && healthvalue <= healthmaxvalue; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        healthlevel[row] += 1<<col;
    }
    lcd.writeCommand(customcharStartAddress + customcharAddressIncrement);
    for (int u=0; u<8; ++u){
        lcd.writeData(healthlevel[u]);
    }


    //Speed icon
    lcd.writeCommand(customcharStartAddress + 2*(customcharAddressIncrement));

    for(int i=0; i<8; i++){
        lcd.writeData(speedicon[i]);
    }



    lcd.writeCommand(customcharStartAddress + 3*(customcharAddressIncrement));

    for (int u=0; u<8; ++u){
        speedlevel[u]=0x00;
        lcd.writeData(speedlevel[u]);
    }

    lcd.writeCommand(customcharStartAddress + 3*(customcharAddressIncrement));
    for (int u=0; u<speedvalue+1 && speedvalue <= speedmaxvalue; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        speedlevel[row] += 1<<col;
    }
    lcd.writeCommand(customcharStartAddress + 3*(customcharAddressIncrement));
    for (int u=0; u<8; ++u){
        lcd.writeData(speedlevel[u]);
    }


    //Defence icon
    lcd.writeCommand(customcharStartAddress + 4*(customcharAddressIncrement));

    for (int i=0; i<8; i++){
        lcd.writeData(defenceicon[i]);
    }


    //Defence data
    lcd.writeCommand(customcharStartAddress + 5*(customcharAddressIncrement));

    for (int u=0; u<8; ++u){
        defencelevel[u]=0x00;
        lcd.writeData(defencelevel[u]);
    }

    lcd.writeCommand(customcharStartAddress + 5*(customcharAddressIncrement));
    for (int u=0; u<defencevalue+1 && defencevalue<=defencemaxvalue; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        defencelevel[row] += 1<<col;
    }
    lcd.writeCommand(customcharStartAddress + 5*(customcharAddressIncrement));
    for (int u=0; u<8; ++u){
        lcd.writeData(defencelevel[u]);
    }


    //Power icon
    lcd.writeCommand(customcharStartAddress + 6*(customcharAddressIncrement));

    for(int i = 0; i < 8; i++){
        lcd.writeData(powericon[i]);
    }


    //Power data
    lcd.writeCommand(customcharStartAddress + 7*(customcharAddressIncrement));

    for (int u = 0; u < 8; ++u){
        powerlevel[u]=0x00;
        lcd.writeData(powerlevel[u]);
    }

    lcd.writeCommand(customcharStartAddress + 7*(customcharAddressIncrement));
    for (int u = 0; u < powervalue+1 && powervalue <= powermaxvalue; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        powerlevel[row] += 1<<col;
    }
    lcd.writeCommand(customcharStartAddress + 7*(customcharAddressIncrement));
    for (int u=0; u<8; ++u){
        lcd.writeData(powerlevel[u]);
    }

    Scroll("Welcome to the RPG!");
    thread_sleep_for(5000);

    Scroll("You leave the village");
    thread_sleep_for(2000);

    //showInventory();


    while(true){

        button1right_counter();
        button2left_counter();
        //button3_counter();

        mainscreen();
        /*if(button3 == false){
            printf("%s\n",inventory[button3press]);
            thread_sleep_for(2000);
            printf("%s\n",inventory[button3press+1]);
        }*/

        

        showInventory();

        sleep();

    
    }
}
