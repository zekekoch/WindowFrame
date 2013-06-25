#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "hsv2rgb.h"

RF24 radio(9,10);// nRF24L01(+) radio
RF24Network network(radio);// Network uses that radio
const uint16_t this_node = 0;// Address of our node
const uint16_t other_node = 1;// Address of the other node

byte ledMode = 0;

const int redPin = 22;
const int greenPin = 4;
const int bluePin = 20;

// Structure of our payload
struct payload_t
{
    unsigned long ms;
    byte mode;
    byte eq[7];
};

void HSVtoRGB(int hue, int sat, int val, int colors[3]) {
    CRGB c;
    hsv2rgb_rainbow(CHSV(hue, sat, val), c);
    colors[0] = c.r;
    colors[1] = c.g;
    colors[2] = c.b;
}

//------------------SETUP------------------
void setup()
{
    delay(1000);
    
    SPI.begin();

    radio.begin();
    radio.setDataRate(RF24_250KBPS);
    radio.printDetails();
    network.begin(90, this_node);
    
    pinMode(redPin, OUTPUT);
    pinMode(greenPin, OUTPUT);
    pinMode(bluePin, OUTPUT);
    
    analogWrite(redPin, 0);
    analogWrite(greenPin, 0);
    analogWrite(bluePin, 0);
    
	Serial.begin(38400);
}

void rainbow()
{
    CRGB c;
    static byte hue = 0;
    hue++;
    hsv2rgb_rainbow(CHSV(hue, 255, 255), c);
    setColor(c);
}

void saturation()
{
    CRGB c;
    static byte hue = 0;
    static bool dir = false;
    if (dir == false)
    {
        hue++;
        if (hue == 255)
            dir = true;
    } else
    {
        hue--;
        if (hue == 0)
            dir = false;
    }
    hsv2rgb_rainbow(CHSV(255, hue, 255), c);
    setColor(c);
}


void setColor(CRGB c)
{
    analogWrite(redPin, c.r);
    analogWrite(greenPin, c.g);
    analogWrite(bluePin, c.b);
}

void musicRainbow(byte eq[7])
{
    CRGB c;
    hsv2rgb_rainbow(CHSV(bass(eq) + 100, 255, 255), c);
    setColor(c);

}

void musicSaturation(byte eq[7])
{
    CRGB c;
    hsv2rgb_spectrum(CHSV(100, 255-bass(eq), 255), c);
    setColor(c);

}

void musicFade(byte eq[7], CRGB baseColor, CRGB highlightColor)
{

    if (bass(eq) > 150) {
        setColor(baseColor);
    } else {
        setColor(highlightColor);
    }
}

byte high(byte eq[7])
{
    int out;
    out = (eq[4] + eq[4]) / 2;
    return (byte)out;    
}

byte bass(byte eq[7])
{
    int out;
    out = (eq[0] + eq[1]) / 2;
    return (byte)out;
}


//------------------MAIN LOOP------------------
void loop() {
    
    // Pump the network regularly
    network.update();
    payload_t payload;
    
    // Is there anything ready for us?
    while ( network.available() )
    {
        // If so, grab it and print it out
        RF24NetworkHeader header;
        network.read(header,&payload,sizeof(payload));
        Serial.print("Received m:");
        Serial.print(payload.mode);
        Serial.print(" eq:");
        for(int i = 0;i<7;i++)
        {
            Serial.print(payload.eq[i]);
            Serial.print(" ");
        }
        Serial.print(" at ");
        Serial.println(payload.ms);
        
       ledMode = payload.mode;
    }
        
    switch(ledMode)
    {
        case 0:
            musicFade(payload.eq, CRGB::LightPink, CRGB::Fuchsia);
            delay(5);
            break;
        case 1:
            setColor(CRGB::AntiqueWhite);
            delay(5);
            break;
        case 2:
        case 3:
        case 4:
            rainbow(); 
            delay(5);
            break;
        case 5: // color_bounce
        case 6: // color_bounceFADE
            musicFade(payload.eq, CRGB::Black, CRGB::Red);
            break;
        case 7: // police_lightsONE
        case 8: // police_lightsALL
            musicFade(payload.eq, CRGB::Blue, CRGB::Red);
            delay(5);
            break;
        //    saturation();
        //    delay(5);
        case 9: // flicker
        case 10:
        case 11:
            musicSaturation(payload.eq);
            delay(5);
            break;
        case 12: // fade_vertical
        case 13: // rule30
            musicRainbow(payload.eq);
            delay(5);
            break;
        case 14: // random_march
        case 15: // rwb_march
        case 17: // color_loop_vardelay
            rainbow(); 
            delay(5);
            break;
        case 101:
            setColor(CRGB::Red);
            break;
        case 103:
            setColor(CRGB::Blue);
            break;
        case 102:
            setColor(CRGB::Green);
            break;
        case 104:
            setColor(CRGB::Yellow);
            break;
        case 105:
            setColor(CRGB::Teal);
            break;
        case 106:
            setColor(CRGB::Violet);
            break;
        default:
            musicRainbow(payload.eq);
            delay(5);
            break;
    }
    /*
     if (ledMode == 12) {fade_vertical(240, 60);}         //--- VERTICAL SOMETHING
     if (ledMode == 16) {radiation(120, 60);}             //--- RADIATION SYMBOL (OR SOME APPROXIMATION)
     if (ledMode == 18) {white_temps();}                  //--- WHITE TEMPERATURES
     if (ledMode == 19) {sin_bright_wave(240, 35);}       //--- SIN WAVE BRIGHTNESS
     if (ledMode == 20) {pop_horizontal(300, 100);}       //--- POP LEFT/RIGHT
     if (ledMode == 21) {quad_bright_curve(240, 100);}    //--- QUADRATIC BRIGHTNESS CURVE
     if (ledMode == 22) {flame();}                        //--- FLAME-ISH EFFECT
     if (ledMode == 23) {rainbow_vertical(10, 20);}       //--- VERITCAL RAINBOW
     if (ledMode == 24) {pacman(100);}                     //--- PACMAN
     
     if (ledMode == 98) {strip_march_ccw(100);}           //--- MARCH WHATEVERS ON THE STRIP NOW CC-W
     if (ledMode == 99) {strip_march_cw(100);}            //--- MARCH WHATEVERS ON THE STRIP NOW C-W
     
     if (ledMode == 888) {demo_mode();}
     */
}

