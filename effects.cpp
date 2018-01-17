#include "effects.h"
#include <FastLED.h>
#include <math.h>

static const uint32_t SPARKLE_FADE_IN_SPEED = 30;   // lower number fades in slower
static const uint32_t SPARKLE_FADE_OUT_SPEED = 6;    // higher number hangs around longer

uint32_t Effect::solidPower(const CRGBPalette16* palette, uint32_t nLEDs) 
{
    uint32_t total = 0;
    for(int i=0; i<NUM_PALETTE_ENTRIES; ++i) {
        total += palette->entries[i].red + palette->entries[i].green + palette->entries[i].blue;
    }


    return nLEDs * uint32_t(20) * total * uint32_t(4) / (uint32_t(255) * uint32_t(NUM_PALETTE_ENTRIES) * uint32_t(7));
}

void SolidEffect::init(uint32_t /*currentTime*/, State* /*state*/, int /*nState*/)
{
    firstCall = true;
}


void SolidEffect::process(uint32_t /*currentTime*/, uint32_t /*deltaTimre*/, State* state, int nState, const CRGBPalette16* palette)
{ 
    if (firstCall) {
        for(int i=0; i<nState; ++i) {
            state[i].r = palette->entries[i%NUM_PALETTE_ENTRIES].red;
            state[i].g = palette->entries[i%NUM_PALETTE_ENTRIES].green;
            state[i].b = palette->entries[i%NUM_PALETTE_ENTRIES].blue;
        }
        firstCall = false;
    }
}


uint32_t SolidEffect::power(const CRGBPalette16* palette, uint32_t nLEDs) const
{
    return solidPower(palette, nLEDs);
}


void SinEffect::init(uint32_t /*currentTime*/, State* /*state*/, int /*nState*/)
{
}


void SinEffect::process(uint32_t currentTime, uint32_t /*deltaTimre*/, State* state, int nState, const CRGBPalette16* palette)
{ 
    float ft = float(currentTime) / 1000.0f;    // current time, seconds
    static const float TIME_MULT = 6.4f;
    static const float DIST_MULT = 0.1f; 

    for(int i=0; i<nState; ++i) {
        float s = sin(ft * TIME_MULT + i * DIST_MULT);
        s = s + 1.0f;                       // 0-2
        uint32_t m = uint32_t(s * 128.0f);  // 0-256

        state[i].r = uint32_t(palette->entries[i%NUM_PALETTE_ENTRIES].red)   * m / uint32_t(256);
        state[i].g = uint32_t(palette->entries[i%NUM_PALETTE_ENTRIES].green) * m / uint32_t(256);
        state[i].b = uint32_t(palette->entries[i%NUM_PALETTE_ENTRIES].blue)  * m / uint32_t(256);
    }
}


uint32_t SinEffect::power(const CRGBPalette16* palette, uint32_t nLEDs) const
{
    // (2+2) / (pi*2)
    return solidPower(palette, nLEDs) * uint32_t(64) / uint32_t(100);
}


void IcicleEffect::init(uint32_t /*currentTime*/, State* /*state*/, int /*nState*/)
{
}


void IcicleEffect::process(uint32_t currentTime, uint32_t /*deltaTimre*/, State* state, int nState, const CRGBPalette16* palette)
{ 
    static const uint32_t LEN = NUM_PALETTE_ENTRIES*2;
    uint32_t lead = (currentTime / 50) % LEN;

    for(int i=0; i<nState; ++i) {

        uint32_t pos = i % LEN;
        uint32_t d = 0;
        if (lead >= pos)
            d = lead - pos;
        else
            d = (lead + LEN) - pos;

        uint32_t b = LEN - d;

        state[i].r = uint32_t(palette->entries[i % NUM_PALETTE_ENTRIES].red)   * b / uint32_t(LEN);
        state[i].g = uint32_t(palette->entries[i % NUM_PALETTE_ENTRIES].green) * b / uint32_t(LEN);
        state[i].b = uint32_t(palette->entries[i % NUM_PALETTE_ENTRIES].blue)  * b / uint32_t(LEN);
    }
}


uint32_t IcicleEffect::power(const CRGBPalette16* palette, uint32_t nLEDs) const
{
    // Keepse everything half lit
    return solidPower(palette, nLEDs) / uint32_t(2);
}


void SparkleEffect::init(uint32_t /*currentTime*/, State* state, int nState)
{
    // Brightness goes from about -400 to 255
    for(int i=0; i<nState; ++i) {
        state[i].init();
        state[i].brightness = -int(random8()) + int(random8());
        state[i].palEntry = random8() % NUM_PALETTE_ENTRIES;
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


uint32_t SparkleEffect::power(const CRGBPalette16* palette, uint32_t nLEDs) const
{
    // Tricky to estimate. 40%??
    return solidPower(palette, nLEDs) * uint32_t(4) / uint32_t(10);
}
