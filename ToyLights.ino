#include <FastLED.h>
#include <Button.h>
#include "palettes.h"
#include "effects.h"

//#define FEATURE_SHUTDOWN

#define LED_PIN     6
#define NUM_LEDS    60
#define BUTTON_0_PIN  9 // Color change
#define BUTTON_1_PIN  10 // Effect

static const uint32_t SHUTDOWN_TIME = uint32_t(26) * uint32_t(60) * uint32_t(1000);
uint32_t shutdownTime = 0;

#define MASTER_BRIGHTNESS   18   // change this to change overall brightness

CRGB leds[NUM_LEDS];
Button button0(BUTTON_0_PIN, Button::INTERNAL_PULLUP);
Button button1(BUTTON_1_PIN, Button::INTERNAL_PULLUP);

State state[NUM_LEDS];
SolidEffect solidEffect;
SinEffect sinEffect;
IcicleEffect icicleEffect;
SparkleEffect sparkleEffect;

int currentPalette = 9;
CRGBPalette16 gPalette = OceanColors_p;

Effect* effect = &sparkleEffect;
int currentEffect = 3;

uint32_t currentTime = 0;


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
        currentPalette = (currentPalette+1) % 15;

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

        effect->init(millis(), state, NUM_LEDS);
    }
    button1.process();
    if (button1.press()) {
        currentEffect = (currentEffect+1) % 4;
        switch(currentEffect) {
            case 0:   effect = &solidEffect;        break;
            case 1:   effect = &sinEffect;          break;
            case 2:   effect = &icicleEffect;       break;
            case 3:   effect = &sparkleEffect;      break;
        }
        effect->init(millis(), state, NUM_LEDS);
    }

#ifdef FEATURE_SHUTDOWN
    if (millis() > shutdownTime) {
        currentPalette = 0;
        for (int i = 0; i < NUM_PALETTE_ENTRIES; i++) {
            gPalette[i] = CRGB::Black;
        }
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


