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

Semaphore button1Semaphore(0,1);
Semaphore button2Semaphore(0,1);
Semaphore button3Semaphore(0,1);

Thread button1Thread;
Thread button2Thread;
Thread button3Thread;

char const *demoStory = "Very long story description."; 

int button3counter = 0;

void button1Fn(){
    button1Semaphore.release();

}

void button2Fn(){
    button2Semaphore.release();

}

void button3Fn(){
    button3Semaphore.release();
}

/*void button1ThreadFn(){
    while(true){
        button1Semaphore.acquire();
        lcd.cls();
        lcd.locate(0,0);
        lcd.printf("Start game");
    }
}*/

/*void Scroll(char const *text){
    while(strlen(text)> 16){
        for(int i = 0; i<(strlen(text)-14); ++i){
            //lcd.cls();
            lcd.locate(0,i);
            lcd.printf("%c", text[i]);
            text = text + 1;
            thread_sleep_for(100);
        }


    }

}*/

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

void buttonpress(char const *text){
    if(button3 == false){
        if(button3counter <strlen(text)){
            button3counter++;

        }
        else if(button3counter == strlen(text)){
            button3counter =0;
        }
    }
}



void button2ThreadFn(){
    while(true){
        button2Semaphore.acquire();
        lcd.cls();
        lcd.locate(0,0);
        Scroll(demoStory);
        //buttonpress(demoStory);

        lcd.locate(0,1);
        lcd.printf("A");

        lcd.locate(3,1);
        lcd.printf("B");

        lcd.locate(6,1);
        lcd.printf("C");

        lcd.locate(9,1);
        lcd.printf("D");

    }
}

void scene1Fn(){
    while(true){
        button2Semaphore.acquire();
        lcd.cls();
        lcd.locate(0,0);
        Scroll("You leave the village. Choose the direction:");

        lcd.locate(0,1);
        lcd.printf("A. Forward");
    }
}

void scene2Fn(){
    while(true){
        button3Semaphore.acquire();
        lcd.cls();
        lcd.locate(0,0);
        printf("You enter a plain.");
        lcd.locate(0,1);
        printf("...");

        //thread_sleep_for(5000);

        /*lcd.cls();
        lcd.locate(0,0);
        Scroll("You encounter a goblin:");

        lcd.locate(0,1);
        lcd.printf("Run");

        lcd.locate(4,1);
        lcd.printf("Hide");

        lcd.locate(9,1);
        lcd.printf("Fight");*/
    }
}

void confirmedFn(){
    while(true){
        
    }
}

 

int main() 
{
    lcd.cls(); 

    //button1Thread.start(button1ThreadFn);
    button2Thread.start(scene1Fn);
    button3Thread.start(scene2Fn);

    button1.fall(button1Fn);
    button2.fall(button2Fn);
    button3.fall(button3Fn);

    lcd.printf("Game title\n");



    while(true){

        sleep();

    
    }
}
