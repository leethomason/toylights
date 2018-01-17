#include <Arduino.h>
#include <FastLED.h>
#include <Button.h>
#include "palettes.h"
#include "effects.h"

//#define FEATURE_SHUTDOWN
//#define FEATURE_EPROM
#define LED_PIN       11
#define NUM_LEDS      60
#define BUTTON_0_PIN  9  // Color change
#define BUTTON_1_PIN  6  // Effect
#define BUTTON_2_PIN  5  // Brightness
#define MAX_POWER 900    // in milliAmps

#ifdef FEATURE_EPROM
#include "EEPROM.h"
#endif

#define ESLOT_INIT      0
#define ESLOT_PALETTE   1
#define ESLOT_EFFECT    2
#define ESLOT_BRIGHT    3

static const uint32_t SHUTDOWN_TIME = uint32_t(26) * uint32_t(60) * uint32_t(1000);
uint32_t shutdownTime = 0;

#define MASTER_BRIGHTNESS   24
#define BRIGHTNESS_1        50
#define BRIGHTNESS_2        100
#define BRIGHTNESS_3        200

CRGB leds[NUM_LEDS];
Button button0(BUTTON_0_PIN, Button::INTERNAL_PULLUP);
Button button1(BUTTON_1_PIN, Button::INTERNAL_PULLUP);
Button button2(BUTTON_2_PIN, Button::INTERNAL_PULLUP);

State state[NUM_LEDS];
SolidEffect solidEffect;
SinEffect sinEffect;
IcicleEffect icicleEffect;
SparkleEffect sparkleEffect;

CRGBPalette16 gPalette = WhiteColors_p;
Effect* effect = &solidEffect;
uint32_t currentTime = 0;

#ifndef FEATURE_EPROM
// M0 doesn't have EEPROM
uint8_t eepromEffect = 0;
uint8_t eepromPalette = 0;
#endif

int setEffect(int id)
{
    id = id % 4;
    switch(id) {
        case 0:        effect = &solidEffect;      break;
        case 1:        effect = &sinEffect;        break;
        case 2:        effect = &icicleEffect;     break;
        case 3:        effect = &sparkleEffect;    break;
    }
    effect->init(millis(), state, NUM_LEDS);
#ifdef FEATURE_EPROM
    EEPROM.update(ESLOT_EFFECT, id);
#else
    eepromEffect = id;
#endif
    resetBrightness();
    return id;
}


int setPalette(int id)
{
    id = id % 15;
    switch (id) {
        case 0:        gPalette = WhiteColors_p;      break;
        case 1:        gPalette = RedColors_p;        break;
        case 2:        gPalette = OrangeColors_p;     break;
        case 3:        gPalette = YellowColors_p;     break;
        case 4:        gPalette = GreenColors_p;      break;
        case 5:        gPalette = BlueColors_p;       break;
        case 6:        gPalette = PurpleColors_p;     break;

        case 7:        gPalette = CloudColors_p;      break;
        case 8:        gPalette = LavaColors_p;       break;
        case 9:        gPalette = OceanColors_p;      break;
        case 10:       gPalette = ForestColors_p;     break;
        case 11:       gPalette = RainbowColors_p;    break;
        case 12:       gPalette = PartyColors_p;      break;
        case 13:       gPalette = HeatColors_p;       break;
        case 14:       gPalette = CastleColors_p;     break;
    }
    effect->init(millis(), state, NUM_LEDS);
#ifdef FEATURE_EPROM
    EEPROM.update(ESLOT_PALETTE, id);
#else
    eepromPalette = id;
#endif
    resetBrightness();
    return id;
}

uint8_t setBrightness(uint8_t value)
{
    //Serial.print("setB "); Serial.println(value);
    uint32_t effectPower = effect->power(&gPalette, NUM_LEDS);
    uint32_t power = effectPower * uint32_t(value) / uint32_t(255);
    //Serial.print("power "); Serial.println(power);
    if (power > MAX_POWER) {
        // power = effectPower * value / 255
        uint32_t newValue = uint32_t(255) * MAX_POWER / effectPower;
        if (newValue > 255) 
            newValue = 255;
        value = uint8_t(newValue);
    }
    //Serial.print("setB "); Serial.println(value);

    FastLED.setBrightness(value);
#ifdef FEATURE_EPROM
    EEPROM.update(ESLOT_BRIGHT, value);
#endif
    return value;
}

void resetBrightness()
{
    setBrightness(MASTER_BRIGHTNESS);
}

int getEffect()   {
#ifdef FEATURE_EPROM
    return EEPROM.read(ESLOT_EFFECT);
#else
    return eepromEffect;
#endif
}

int getPalette()  {
#ifdef FEATURE_EPROM
    return EEPROM.read(ESLOT_PALETTE);
#else
    return eepromPalette;
#endif
}

uint8_t getBrightness() {
#ifdef FEATURE_EPROM
    return EEPROM.read(ESLOT_BRIGHT);
#else
    return FastLED.getBrightness();
#endif
}

void setup() {
    //Serial.begin(19200);
    //while(!Serial) {
    //  delay(100);
    //}

    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
    FastLED.setBrightness(MASTER_BRIGHTNESS);
    for (int i = 0; i < NUM_LEDS; ++i) {
        state[i].init();
        state[i].brightness = -(random8());
        state[i].palEntry = i % NUM_PALETTE_ENTRIES;
        leds[i].setRGB(state[i].r, state[i].g, state[i].b);
    }
    FastLED.show();
    currentTime = millis();
    shutdownTime = currentTime + SHUTDOWN_TIME;
    button2.setHoldRepeats(true);
    button2.setHoldThreshold(1000);

#ifdef FEATURE_EPROM
    if (EEPROM.read(ESLOT_INIT) != 42) {
        EEPROM.update(ESLOT_INIT, 42);
        EEPROM.update(ESLOT_PALETTE, 0);
        EEPROM.update(ESLOT_EFFECT, 0);
        EEPROM.update(ESLOT_BRIGHT, 0);
    }
    setPalette(EEPROM.read(ESLOT_PALETTE));
    setEffect(EEPROM.read(ESLOT_EFFECT));
    setBrightness(EEPROM.read(ESLOT_BRIGHT)); 
#else
    setPalette(0);
    setEffect(0);
    setBrightness(MASTER_BRIGHTNESS);
#endif
}


void loop() {
    button0.process();
    if (button0.press()) {
        shutdownTime = currentTime + SHUTDOWN_TIME;
        setPalette(getPalette() + 1);
        shutdownTime = currentTime + SHUTDOWN_TIME;
    }
    button1.process();
    if (button1.press()) {
        setEffect(getEffect() + 1);
        shutdownTime = currentTime + SHUTDOWN_TIME;
    }

    button2.process();
    if (button2.press()) {
        resetBrightness();
        shutdownTime = currentTime + SHUTDOWN_TIME;
    }
    if (button2.held()) {
        switch(button2.nHolds()) {
        case 1:
            setBrightness(BRIGHTNESS_1);
            break;
        case 2:
            setBrightness(BRIGHTNESS_2);
            break;
        default:
            setBrightness(BRIGHTNESS_3);
            break;
        }
        shutdownTime = currentTime + SHUTDOWN_TIME;
    }

#ifdef FEATURE_SHUTDOWN
    if (millis() > shutdownTime) {
        if (leds[0].getRGB() != CRGB::black) {
            for (int i = 0; i < NUM_PALETTE_ENTRIES; i++) {
                for (int i = 0; i < NUM_LEDS; ++i) {
                    leds[i].setRGB(0, 0, 0);
                }
                FastLED.show();
            }
        }
        return;
    }
#endif

    uint32_t delta = millis() - currentTime;
    if (delta >= 20) {
        currentTime = millis();
        effect->process(currentTime, delta, state, NUM_LEDS, &gPalette);

        for (int i = 0; i < NUM_LEDS; ++i) {
            leds[i].setRGB(state[i].r, state[i].g, state[i].b);
        }
        FastLED.show();
    }
}
