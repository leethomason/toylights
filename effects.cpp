#include "effects.h"
#include <FastLED.h>

static const uint32_t SPARKLE_FADE_IN_SPEED = 30;   // lower number fades in slower
static const uint32_t SPARKLE_FADE_OUT_SPEED = 6;    // higher number hangs around longer

void SparkleEffect::init(uint32_t /*currentTime*/, State* state, int nState)
{
    // Brightness goes from about -400 to 255
    for(int i=0; i<nState; ++i) {
        state[i].init();
        state[i].brightness = -int(random8()) + int(random8());
        state[i].palEntry = random8() % 16;
    }
}

void SparkleEffect::process(uint32_t /*currentTime*/, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette)
{
    int fadeInSpeed = int(SPARKLE_FADE_IN_SPEED * deltaTime / uint32_t(50));     // 50ms step is legacy
    int fadeOutSpeed = int(SPARKLE_FADE_OUT_SPEED * deltaTime / uint32_t(50));

    for (int i = 0; i < nState; ++i) {
        State& s = state[i];

        if (s.dir > 0) {
            s.brightness += fadeInSpeed;
            if (s.brightness > 255) {
                s.brightness = 255;
                s.dir = -1;
            }
        }
        else {
            s.brightness -= fadeOutSpeed;
            if (s.brightness < 0) {
                s.brightness = -(random16() % 800);
                s.dir = 1;
                s.palEntry = random16() % NUM_PALETTE_ENTRIES;
            }
        }
        s.r = s.g = s.b = 0;
        if (s.brightness > 0) {
            s.r = uint32_t(s.brightness) * uint32_t(palette->entries[s.palEntry].red)   / uint32_t(255);
            s.g = uint32_t(s.brightness) * uint32_t(palette->entries[s.palEntry].green) / uint32_t(255);
            s.b = uint32_t(s.brightness) * uint32_t(palette->entries[s.palEntry].blue)  / uint32_t(255);
        }
    }
}