#ifndef EFFECTS_INCLUDED
#define EFFECTS_INCLUDED

#include <stdint.h>

#define NUM_PALETTE_ENTRIES 16

class CRGBPalette16;

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


class Effect {
public:
    Effect() {}

    virtual void init(uint32_t currentTime, State* state, int nState) = 0;
    virtual void process(uint32_t currentTime, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette) = 0;
    // Approximate draw at full brightness.
    virtual uint32_t power(const CRGBPalette16* palette, uint32_t nLEDs) const = 0;

protected:
    static uint32_t solidPower(const CRGBPalette16* palette, uint32_t power);
};

class SolidEffect : public Effect {
public:
    SolidEffect() {}

    virtual void init(uint32_t currentTime, State* state, int nState);
    virtual void process(uint32_t currentTime, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette);
    virtual uint32_t power(const CRGBPalette16* palette, uint32_t nLEDs) const;

private:
    bool firstCall;
};

class SinEffect : public Effect {
public:
    SinEffect() {}

    virtual void init(uint32_t currentTime, State* state, int nState);
    virtual void process(uint32_t currentTime, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette);
    virtual uint32_t power(const CRGBPalette16* palette, uint32_t nLEDs) const;
};

class IcicleEffect : public Effect {
public:
    IcicleEffect() {}

    virtual void init(uint32_t currentTime, State* state, int nState);
    virtual void process(uint32_t currentTime, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette);
    virtual uint32_t power(const CRGBPalette16* palette, uint32_t nLEDs) const;
};

class SparkleEffect : public Effect {
public:
    SparkleEffect() {}

    virtual void init(uint32_t currentTime, State* state, int nState);
    virtual void process(uint32_t currentTime, uint32_t deltaTime, State* state, int nState, const CRGBPalette16* palette);
    virtual uint32_t power(const CRGBPalette16* palette, uint32_t nLEDs) const;
};

#endif // EFFECTS_INCLUDED
