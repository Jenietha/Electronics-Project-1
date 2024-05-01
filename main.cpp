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
char health[8] ={ 0x00,
                  0x0A,
                  0x1F,
                  0x1F,
                  0x1F,
                  0x0E,
                  0x04,
                  0x00

};

//character for player's speed and action icon for run/dodge
char speed[8] = { 0x00,
                  0x07,
                  0x0F,
                  0x1E,
                  0x1C,
                  0x0E,
                  0x00,
                  0x00
};

//character for player's defence and action icon for defending
char defence[8] = { 0x00,
                    0x1F,   
                    0x1F,
                    0x1F,
                    0x0E,
                    0x04,
                    0x00,
                    0x00

};

//character for player's power and action icon for attack
char power[8] = { 0x04,
                  0x0E,
                  0x0E,
                  0x0E,
                  0x0E,
                  0x1F,
                  0x04,
                  0x04};



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

    lcd.writeCommand(0x40);

    for(int i=0;i<8;i++){
        lcd.writeData(health[i]);
    }

    lcd.locate(12,1);
    lcd.putc(0);

    lcd.writeCommand(0x40 +8);

    for(int i=0; i<8; i++){
        lcd.writeData(speed[i]);
    }

    lcd.locate(13,1);
    lcd.putc(1);

    lcd.writeCommand(0x40 +16);

    for (int i=0; i<8; i++){
        lcd.writeData(defence[i]);
    }

    lcd.locate(14,1);
    lcd.putc(2);

    lcd.writeCommand(0x40 +24);

    for(int i=0; i<8; i++){
        lcd.writeData(power[i]);
    }

    lcd.locate(15,1);
    lcd.putc(3);



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
