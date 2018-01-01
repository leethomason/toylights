#include <FastLED.h>
#include <Button.h>

#define LED_PIN     6
#define NUM_LEDS    60
#define BUTTON_PIN  10
#define NUM_PALETTE_ENTRIES 16

static const uint32_t SHUTDOWN_TIME = uint32_t(26) * uint32_t(60) * uint32_t(1000);
uint32_t shutdownTime = 0;

#define MASTER_BRIGHTNESS   18   // change this to change overall brightness
#define FADE_IN_SPEED       30   // lower number fades in slower
#define FADE_OUT_SPEED      6    // higher number hangs around longer

CRGB leds[NUM_LEDS];
Button button(BUTTON_PIN, Button::INTERNAL_PULLUP, Button::DEFAULT_BOUNCE_DURATION);

int currentPalette = 1;
CRGBPalette16 gPalette = OceanColors_p;

struct State {
    int palEntry;
    int dir;
    int brightness;
    uint8_t r, g, b;

    void init() {
        palEntry = 0;
        dir = 1;
        brightness = 0;
        r = g = b = 0;
    }
};
State state[NUM_LEDS];
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
    button.process();
    if (button.press()) {
        shutdownTime = currentTime + SHUTDOWN_TIME;
        currentPalette = (currentPalette + 1) % 7;

        switch (currentPalette) {
        case 0:
            for (int i = 0; i < NUM_PALETTE_ENTRIES; i++) {
                gPalette[i] = CRGB::Black;
            }
            break;
        case 1:
            gPalette = OceanColors_p;
            break;
        case 2:
            gPalette = CloudColors_p;
            break;
        case 3:
            gPalette = ForestColors_p;
            break;
        case 4:
            gPalette = RainbowColors_p;
            break;
        case 5:
            gPalette = PartyColors_p;
            break;
        case 6:
            gPalette[0] = CRGB::DeepPink;
            gPalette[1] = CRGB::HotPink;
            gPalette[2] = CRGB::LightPink;
            gPalette[3] = CRGB::Pink;
            gPalette[4] = CRGB::Lavender;
            gPalette[5] = CRGB::LavenderBlush;
            gPalette[6] = CRGB::Amethyst;
            gPalette[7] = CRGB::Coral;
            gPalette[8] = CRGB::MidnightBlue;
            gPalette[9] = CRGB::Plum;
            gPalette[10] = CRGB::Violet;
            gPalette[11] = CRGB::DeepPink;
            gPalette[12] = CRGB::HotPink;
            gPalette[13] = CRGB::LightPink;
            gPalette[14] = CRGB::MediumPurple;
            gPalette[15] = CRGB::Purple;
            break;
        }
    }

    if (millis() > shutdownTime) {
        currentPalette = 0;
        for (int i = 0; i < NUM_PALETTE_ENTRIES; i++) {
            gPalette[i] = CRGB::Black;
        }
    }

    uint32_t delta = millis() - currentTime;
    if (delta >= 50) {
        currentTime = millis();
        for (int i = 0; i < NUM_LEDS; ++i) {
            State& s = state[i];

            if (s.dir > 0) {
                s.brightness += FADE_IN_SPEED;
                if (s.brightness > 255) {
                    s.brightness = 255;
                    s.dir = -1;
                }
            }
            else {
                s.brightness -= FADE_OUT_SPEED;
                if (s.brightness < 0) {
                    s.brightness = -(random16() % 800);
                    s.dir = 1;
                    s.palEntry = random16() % NUM_PALETTE_ENTRIES;
                }
            }
            s.r = s.g = s.b = 0;
            if (s.brightness > 0) {
                s.r = uint32_t(s.brightness) * uint32_t(gPalette[s.palEntry].red)   / uint32_t(255);
                s.g = uint32_t(s.brightness) * uint32_t(gPalette[s.palEntry].green) / uint32_t(255);
                s.b = uint32_t(s.brightness) * uint32_t(gPalette[s.palEntry].blue)  / uint32_t(255);
            }
            leds[i].setRGB(s.r, s.g, s.b);
        }
        FastLED.show();
    }

}


