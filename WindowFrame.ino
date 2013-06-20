#include <RF24Network.h>
#include <RF24.h>
#include <SPI.h>
#include "hsv2rgb.h"

RF24 radio(9,10);// nRF24L01(+) radio
RF24Network network(radio);// Network uses that radio
const uint16_t this_node = 0;// Address of our node
const uint16_t other_node = 1;// Address of the other node

byte ledMode = 0;

const int redPin = 4;
const int greenPin = 22;
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

void musicRainbow(byte bass)
{
    CRGB c;
    hsv2rgb_rainbow(CHSV(bass + 100, 255, 255), c);
    setColor(c);

}

void musicSaturation(byte bass)
{
    CRGB c;
    hsv2rgb_spectrum(CHSV(100, 255-bass, 255), c);
    setColor(c);

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
    
    
    int high, bass;
    bass = (payload.eq[0] + payload.eq[1]) / 2;
    high = (payload.eq[4] + payload.eq[4]) /2;
    switch(ledMode)
    {
        case 0:
            
            //Serial.print("bass: ");
            //Serial.print(bass);
            //Serial.println();

            if (bass > 150)
            {
                setColor(CRGB::LightPink);
            }
            else
            {
                setColor(CRGB::Fuchsia);
            }
            
            //    bass > 150 ? fillSolid(1, highlightColor) : fillSolid(1, baseColor);
            //    bass > 200 ? fillSolid(2, highlightColor) : fillSolid(2, baseColor);
            //    high > 150 ? fillSolid(3, highlightColor) : fillSolid(3, baseColor);
            //    high > 100 ? fillSolid(4, highlightColor) : fillSolid(4, baseColor);
            //    high > 200 ? fillSolid(5, highlightColor) : fillSolid(5, baseColor);
            //    high > 200 ? fillSolid(6, highlightColor) : fillSolid(6, baseColor);

            break;
        case 1:
            rainbow();
            delay(5);
            break;
        case 2:
            setColor(CRGB::HotPink);
            break;
        case 3:
            setColor(CRGB::DeepPink);
            break;
        case 4:
            saturation();
            delay(5);
            break;
        case 5:
            setColor(CRGB::Fuchsia);
            break;
        case 6:
            musicRainbow(bass);
            break;
        case 7:
            musicSaturation(bass);
            break;
        default:
            musicRainbow(bass);
            break;
    }
    /*
     if (ledMode == 2) {rainbow_fade(20);}                //---STRIP RAINBOW FADE
     if (ledMode == 3) {rainbow_loop(10, 20);}            //---RAINBOW LOOP
     if (ledMode == 4) {random_burst(20);}                //---RANDOM
     if (ledMode == 5) {color_bounce(20);}                //---CYLON v1
     if (ledMode == 6) {color_bounceFADE(20);}            //---CYLON v2
     if (ledMode == 7) {police_lightsONE(40);}            //---POLICE SINGLE
     if (ledMode == 8) {police_lightsALL(40);}            //---POLICE SOLID
     if (ledMode == 9) {flicker(200,255);}                //---STRIP FLICKER
     if (ledMode == 10) {pulse_one_color_all(0, 10);}     //--- PULSE COLOR BRIGHTNESS
     if (ledMode == 11) {pulse_one_color_all_rev(0, 10);} //--- PULSE COLOR SATURATION
     if (ledMode == 12) {fade_vertical(240, 60);}         //--- VERTICAL SOMETHING
     if (ledMode == 13) {rule30(100);}                    //--- CELL AUTO - RULE 30 (RED)
     if (ledMode == 14) {random_march(30);}               //--- MARCH RANDOM COLORS
     if (ledMode == 15) {rwb_march(50);}                  //--- MARCH RWB COLORS
     if (ledMode == 16) {radiation(120, 60);}             //--- RADIATION SYMBOL (OR SOME APPROXIMATION)
     if (ledMode == 17) {color_loop_vardelay();}          //--- VARIABLE DELAY LOOP
     if (ledMode == 18) {white_temps();}                  //--- WHITE TEMPERATURES
     if (ledMode == 19) {sin_bright_wave(240, 35);}       //--- SIN WAVE BRIGHTNESS
     if (ledMode == 20) {pop_horizontal(300, 100);}       //--- POP LEFT/RIGHT
     if (ledMode == 21) {quad_bright_curve(240, 100);}    //--- QUADRATIC BRIGHTNESS CURVE
     if (ledMode == 22) {flame();}                        //--- FLAME-ISH EFFECT
     if (ledMode == 23) {rainbow_vertical(10, 20);}       //--- VERITCAL RAINBOW
     if (ledMode == 24) {pacman(100);}                     //--- PACMAN
     
     if (ledMode == 98) {strip_march_ccw(100);}           //--- MARCH WHATEVERS ON THE STRIP NOW CC-W
     if (ledMode == 99) {strip_march_cw(100);}            //--- MARCH WHATEVERS ON THE STRIP NOW C-W
     
     if (ledMode == 101) {fillSolid(255,0,0);}    //---101- STRIP SOLID RED
     if (ledMode == 102) {fillSolid(0,255,0);}    //---102- STRIP SOLID GREEN
     if (ledMode == 103) {fillSolid(0,0,255);}    //---103- STRIP SOLID BLUE
     if (ledMode == 104) {fillSolid(255,255,0);}  //---104- STRIP SOLID YELLOW
     if (ledMode == 105) {fillSolid(0,255,255);}  //---105- STRIP SOLID TEAL?
     if (ledMode == 106) {fillSolid(255,0,255);}  //---106- STRIP SOLID VIOLET?
     
     if (ledMode == 888) {demo_mode();}
     */
}

