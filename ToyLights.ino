#include <FastLED.h>
#include <Button.h>
#include "palettes.h"
#include "effects.h"

#define LED_PIN     6
#define NUM_LEDS    60
#define BUTTON_0_PIN  9

static const uint32_t SHUTDOWN_TIME = uint32_t(26) * uint32_t(60) * uint32_t(1000);
uint32_t shutdownTime = 0;

#define MASTER_BRIGHTNESS   18   // change this to change overall brightness

CRGB leds[NUM_LEDS];
Button button0(BUTTON_0_PIN, Button::INTERNAL_PULLUP, Button::DEFAULT_BOUNCE_DURATION);

int currentPalette = 0;
CRGBPalette16 gPalette = OceanColors_p;

State state[NUM_LEDS];
uint32_t currentTime = 0;

SparkleEffect sparkleEffect;

void setup() {
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
}

void loop() {
    button0.process();
    if (button0.press()) {
        shutdownTime = currentTime + SHUTDOWN_TIME;
        ++currentPalette;
        currentPalette = (currentPalette) % 15;

        switch (currentPalette) {
        case 0:     gPalette = WhiteColors_p;      break;
        case 1:     gPalette = RedColors_p;        break;
        case 2:     gPalette = OrangeColors_p;     break;
        case 3:     gPalette = YellowColors_p;     break;
        case 4:     gPalette = GreenColors_p;      break;
        case 5:     gPalette = BlueColors_p;       break;
        case 6:     gPalette = PurpleColors_p;     break;

        case 7:     gPalette = CloudColors_p;      break;
        case 8:     gPalette = LavaColors_p;       break;
        case 9:     gPalette = OceanColors_p;      break;
        case 10:    gPalette = ForestColors_p;     break;
        case 11:    gPalette = RainbowColors_p;    break;
        case 12:    gPalette = PartyColors_p;      break;
        case 13:    gPalette = HeatColors_p;       break;
        case 14:    gPalette = CastleColors_p;     break;
        }

        sparkleEffect.init(millis(), state, NUM_LEDS);
    }

    if (millis() > shutdownTime) {
        currentPalette = 0;
        for (int i = 0; i < NUM_PALETTE_ENTRIES; i++) {
            gPalette[i] = CRGB::Black;
        }
    }

    uint32_t delta = millis() - currentTime;
    if (delta >= 20) {
        currentTime = millis();
        sparkleEffect.process(currentTime, delta, state, NUM_LEDS, &gPalette);

        for (int i = 0; i < NUM_LEDS; ++i) {
            leds[i].setRGB(state[i].r, state[i].g, state[i].b);
        }
        FastLED.show();
    }
}


