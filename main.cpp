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


DigitalIn buttonright(D8, PullUp);
DigitalIn buttonleft(D10, PullUp);
InterruptIn button3(D7, PullUp);
InterruptIn buttonInventory(D12, PullUp);

Semaphore button3Semaphore(0,1);

Thread button3Thread;

TextLCD lcd(D0, D1, D2, D3, D4, D5, TextLCD::LCD16x2); // Connect these nucleo pins to RS, E, D4, D5, D6 and D7 pins of the LCD

int button1_right = 1;
int button2_left = 0;
//int cursorposition = 0;

int choice1 = 0x7F;
int choice2 = 0x7E;
int choice3 = 0x25;

int healthvalue=40;
int speedvalue=40;
int defencevalue=40;
int powervalue=40;

//char inventory[3] = {"Knife", "Plank", "Stick", "Armor"};

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

/*int buttontotalcounter(){
    return button1 - button2;
}*/

//character for player's health
char healthicon[8] ={ 0x00,
                  0x0A,
                  0x1F,
                  0x1F,
                  0x1F,
                  0x0E,
                  0x04,
                  0x00

};

char healthlevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

};

//character for player's speed and action icon for run/dodge
char speedicon[8] = { 0x00,
                  0x07,
                  0x0F,
                  0x1E,
                  0x1C,
                  0x0E,
                  0x00,
                  0x00
};

char speedlevel[8] = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

};

//character for player's defence and action icon for defending
char defenceicon[8] = { 0x00,
                    0x1F,   
                    0x1F,
                    0x1F,
                    0x0E,
                    0x04,
                    0x00,
                    0x00

};
char defencelevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

};

//character for player's power and action icon for attack
char powericon[8] = { 0x04,
                  0x0E,
                  0x0E,
                  0x0E,
                  0x0E,
                  0x1F,
                  0x04,
                  0x04};

char powerlevel[8] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00

};

//Functions to scroll text
void showletters(int printStart, int startLetter, char const *text){
    lcd.locate(printStart,0);
    for(int letter = startLetter; letter<=startLetter+15; ++letter){
        lcd.printf("%c", text[letter]);
    }
    lcd.printf(" ");
    thread_sleep_for(50);
}

void Scroll(char const *text){
    for (int letter = 0; letter<= strlen(text)-16; ++letter){
        showletters(0,letter, text);
        thread_sleep_for(500);
    }
}

//Function to blink characters - to show they are selected
void blinktext(int option1, int option2, int option3){

    if (abs(button1_right-button2_left) % 3 == 0 && abs(button1_right-button2_left) % 2 != 0){
        lcd.locate(6,1);
        lcd.putc(option3);
        thread_sleep_for(500);
        lcd.locate(6,1);
        lcd.printf(" ");
        thread_sleep_for(500);
    }
    else{
        if(abs(button1_right-button2_left) % 2 == 0){
            lcd.locate(3,1);
            lcd.putc(option2);
            thread_sleep_for(500);
            lcd.locate(3,1);
            lcd.printf(" ");
            thread_sleep_for(500);
        }
        else{
            lcd.locate(0,1);
            lcd.putc(option1);
            thread_sleep_for(500);
            lcd.locate(0,1);
            lcd.printf(" ");
            thread_sleep_for(500);
        }
    }
}

/*void showInventory(){
    if(buttonInventory == false){
        for(int i = 0; i<4; i=i+2){
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf(inventory[i]);
            lcd.locate(0,1);
            lcd.printf(inventory[++i]);
            thread_sleep_for(2000);
        }
    }
}*/

void button3Fn(){
    button3Semaphore.release();
}

/*void button3ThreadFn(){
    
    while (true){
        button3Semaphore.acquire();
        if (button1_right % 3 == 0 && button1_right % 2 != 0){
            lcd.cls();
            lcd.locate(0,0);
            lcd.printf("Scene 2C");
         
        }
        else{
            if(button1_right % 2 == 0){
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Scene 2B");
        }
            else{
                lcd.cls();
                lcd.locate(0,0);
                lcd.printf("Scene 2A");
            }
        }
}

}*/


int main() 
{
    //Scroll("Very long scene description");

    //Health icon
    lcd.writeCommand(0x40);

    for(int i=0;i<8;i++){
        lcd.writeData(healthicon[i]);
    }

    lcd.locate(8,1);
    lcd.putc(0);

    //health data
    lcd.writeCommand(0x40+8);

    for (int u=0; u<8; ++u){
        healthlevel[u]=0x00;
        lcd.writeData(healthlevel[u]);
    }

    lcd.writeCommand(0x40+8);
    for (int u=0; u<healthvalue+1 && healthvalue<=40; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        healthlevel[row] += 1<<col;
    }
    lcd.writeCommand(0x40+8);
    for (int u=0; u<8; ++u){
        lcd.writeData(healthlevel[u]);
    }


    lcd.locate(9,1);
    lcd.putc(1);

    //Speed icon
    lcd.writeCommand(0x40 +16);

    for(int i=0; i<8; i++){
        lcd.writeData(speedicon[i]);
    }

    lcd.locate(10,1);
    lcd.putc(2);


    lcd.writeCommand(0x40+24);

    for (int u=0; u<8; ++u){
        speedlevel[u]=0x00;
        lcd.writeData(speedlevel[u]);
    }

    lcd.writeCommand(0x40+24);
    for (int u=0; u<speedvalue+1 && speedvalue<=40; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        speedlevel[row] += 1<<col;
    }
    lcd.writeCommand(0x40+24);
    for (int u=0; u<8; ++u){
        lcd.writeData(speedlevel[u]);
    }

    lcd.locate(11,1);
    lcd.putc(3);

    //Defence icon
    lcd.writeCommand(0x40 +32);

    for (int i=0; i<8; i++){
        lcd.writeData(defenceicon[i]);
    }

    lcd.locate(12,1);
    lcd.putc(4);

    //Defence data
    lcd.writeCommand(0x40+40);

    for (int u=0; u<8; ++u){
        defencelevel[u]=0x00;
        lcd.writeData(defencelevel[u]);
    }

    lcd.writeCommand(0x40+40);
    for (int u=0; u<defencevalue+1 && defencevalue<=40; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        defencelevel[row] += 1<<col;
    }
    lcd.writeCommand(0x40+40);
    for (int u=0; u<8; ++u){
        lcd.writeData(defencelevel[u]);
    }

    lcd.locate(13,1);
    lcd.putc(5);

    //Power icon
    lcd.writeCommand(0x40 +48);

    for(int i=0; i<8; i++){
        lcd.writeData(powericon[i]);
    }

    lcd.locate(14,1);
    lcd.putc(6);

    //Power data
    lcd.writeCommand(0x40+56);

    for (int u=0; u<8; ++u){
        defencelevel[u]=0x00;
        lcd.writeData(defencelevel[u]);
    }

    lcd.writeCommand(0x40+40);
    for (int u=0; u<defencevalue+1 && defencevalue<=40; ++u){
        int row = 7-((u-1)/5);
        int col = 4-((u-1)%5);
        defencelevel[row] += 1<<col;
    }
    lcd.writeCommand(0x40+40);
    for (int u=0; u<8; ++u){
        lcd.writeData(defencelevel[u]);
    }

    lcd.locate(13,1);
    lcd.putc(5);

    

    

    /*lcd.writeCommand(0x40 +32);

    for (int i=0; i<8; i++){
        lcd.writeData(defenceicon[i]);
    }

    lcd.locate(12,1);
    lcd.putc(4);

    lcd.writeCommand(0x40 +24);

    for(int i=0; i<8; i++){
        lcd.writeData(powericon[i]);
    }

    lcd.locate(15,1);
    lcd.putc(3);*/



    while(true){

        //buttonconfirm(button1_right);

        /*while(button1 == false){
            printf("Right button count: %d\n", button1_right);
            thread_sleep_for(1000);
        }

        while(button2 == false){
            printf("Left button count: %d\n", button2_left);
            thread_sleep_for(1000);
        }*/

        //button3Thread.start(button3Fn);
        //button3.fall(button3ThreadFn);

        button1right_counter();
        button2left_counter();
        //showInventory();

        lcd.locate(0,0);
        //Scroll("Very long scene description");
        lcd.locate(0,1);
        //lcd.printf("%c",choice1);
        lcd.putc(choice1);
        lcd.locate(3,1);
        //lcd.printf("%c",choice2);
        lcd.putc(choice2);
        lcd.locate(6,1);
        //lcd.printf("%c",choice3);
        lcd.putc(choice3);

        blinktext(choice1, choice2, choice3);

        

        sleep();

    
    }
}
