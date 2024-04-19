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


InterruptIn button1(D8, PullUp);
InterruptIn button2(D10, PullUp);
InterruptIn button3(D7, PullUp);

TextLCD lcd(D0, D1, D2, D3, D4, D5, TextLCD::LCD16x2); // Connect these nucleo pins to RS, E, D4, D5, D6 and D7 pins of the LCD

int button1_right = 1;
int button2_left = 0;
//int cursorposition = 0;

const char choice1 = 'a';
const char choice2 = 'b';
const char choice3 = 'c';

int button1right_counter(){
    if(button1 == false){
        ++button1_right;
    }
    return button1_right;
}

int button2left_counter(){
    if(button2 == false){
        ++button2_left;    
    }
    return button2_left;
}




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
    }
}

//Function to blink characters - to show they are selected
void blinktext(const char text1, const char text2, const char text3){

    if (button1_right % 3 == 0 && button1_right % 2 != 0){
        lcd.locate(6,1);
        lcd.printf("%c",text3);
        thread_sleep_for(500);
        lcd.locate(6,1);
        lcd.printf(" ");
        thread_sleep_for(500);
    }
    else{
        if(button1_right % 2 == 0){
            lcd.locate(3,1);
            lcd.printf("%c",text2);
            thread_sleep_for(500);
            lcd.locate(3,1);
            lcd.printf(" ");
            thread_sleep_for(500);
        }
        else{
            lcd.locate(0,1);
            lcd.printf("%c", text1);
            thread_sleep_for(500);
            lcd.locate(0,1);
            lcd.printf(" ");
            thread_sleep_for(500);
        }
    }
}

void buttonconfirm(int button1_right){
    while(button3 == false){
        if(button1_right % 3 == 0 && button1_right % 2 != 0){
            lcd.printf("Scene 2C");
        }
        else{
            if(button1_right % 2 == 0){
                lcd.printf("Scene 2B");
            }
            if(button1_right !=0){
                lcd.printf("Scene 2A");
            }
        }
    }

}


 

int main() 
{

    while(true){

        button1right_counter();
        button2left_counter();
        buttonconfirm(button1_right);

        while(button1 == false){
            printf("Right button count: %d\n", button1_right);
            thread_sleep_for(1000);
        }

        while(button2 == false){
            printf("Left button count: %d\n", button2_left);
            thread_sleep_for(1000);
        }
        

        lcd.locate(0,0);
        lcd.printf("Scene 1");
        lcd.locate(0,1);
        lcd.printf("%c",choice1);
        lcd.locate(3,1);
        lcd.printf("%c",choice2);
        lcd.locate(6,1);
        lcd.printf("%c",choice3);

        blinktext(choice1, choice2, choice3);

        

        sleep();

    
    }
}
