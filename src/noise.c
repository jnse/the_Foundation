#include "the_Foundation/noise.h"
#include "the_Foundation/array.h"
#include "the_Foundation/math.h"

struct Impl_Noise {
    iVec2    size;
    float    scale; // normalizing output values
    iFloat3 *gradients;
};

iDefineTypeConstructionArgs(Noise, (iVec2 size), size)

static inline iFloat3 *gradient_Noise_(const iNoise *d, const iVec2 pos) {
    return d->gradients + (d->size.x * pos.y + pos.x);
}

void init_Noise(iNoise *d, iVec2 size) {
    d->size = add_I2(size, one_I2()); // gradients at cell corners
    d->scale = 1.45f;
    d->gradients = malloc(sizeof(iFloat3) * (size_t) prod_I2(d->size));
    for (int i = 0; i < prod_I2(d->size); ++i) {
        const float angle = iRandomf() * iMathPif * 2.f;
        d->gradients[i] = init_F3(cosf(angle), sinf(angle), 0.f);
    }
}

void deinit_Noise(iNoise *d) {
    free(d->gradients);
}

static inline float dotGradient_Noise_(const iNoise *d, const int x, int y, const iFloat3 b) {
    return dot_F3(sub_F3(b, initi_F3(x, y, 0)), *gradient_Noise_(d, init_I2(x, y)));
}

static inline float hermite_(float a, float b, float w) {
    w = iClamp(w, 0, 1);
    return a + (b - a) * (w * w * (3 - 2 * w));
}

iVec2 size_Noise(const iNoise *d) {
    return sub_I2(d->size, one_I2());
}

float eval_Noise(const iNoise *d, float normX, float normY) {
    const float x = normX * (d->size.x - 1);
    const float y = normY * (d->size.y - 1);
    const iVec2 c0 = init_I2((int) x, (int) y);
    const iVec2 c1 = add_I2(c0, init_I2(1, 1));
    if (any_Bool2(less_I2(c0, zero_I2())) || any_Bool2(greaterEqual_I2(c1, d->size))) {
        return 0.f;
    }
    const iFloat3 pos = init_F3(x, y, 0.f);
    const float s0 = hermite_(
        dotGradient_Noise_(d, c0.x, c0.y, pos), dotGradient_Noise_(d, c1.x, c0.y, pos), x - c0.x);
    const float s1 = hermite_(
        dotGradient_Noise_(d, c0.x, c1.y, pos), dotGradient_Noise_(d, c1.x, c1.y, pos), x - c0.x);
    return hermite_(s0, s1, y - c0.y) * d->scale;
}

/*-----------------------------------------------------------------------------------------------*/

iDeclareType(CombinedNoisePart)

struct Impl_CombinedNoisePart {
    float weight;
    float offset;
    iNoise noise;
};

struct Impl_CombinedNoise {
    iArray parts;
};

iDefineTypeConstructionArgs(CombinedNoise,
                            (const iNoiseComponent *components, size_t count),
                            components, count)

void init_CombinedNoise(iCombinedNoise *d, const iNoiseComponent *components, size_t count) {
    init_Array(&d->parts, sizeof(iCombinedNoisePart));
    for (size_t i = 0; i < count; ++i) {
        iCombinedNoisePart part = { .weight = components[i].weight,
                                    .offset = components[i].offset };
        init_Noise(&part.noise, components[i].size);
        pushBack_Array(&d->parts, &part);
    }
}

void deinit_CombinedNoise(iCombinedNoise *d) {
    iForEach(Array, i, &d->parts) {
        deinit_Noise(&((iCombinedNoisePart *) i.value)->noise);
    }
    deinit_Array(&d->parts);
}

float eval_CombinedNoise(const iCombinedNoise *d, float normX, float normY) {
    float value = 0.f;
    iConstForEach(Array, i, &d->parts) {
        const iCombinedNoisePart *part = i.value;
        value += part->weight * eval_Noise(&part->noise, normX, normY) + part->offset;
    }
    return value;
}

iFloat3 randomCoord_CombinedNoise(const iCombinedNoise *d, iBool (*rangeCheck)(float)) {
    const int maxAttempts = 1000;
    for (int i = 0; i < maxAttempts; ++i) {
        iFloat3 pos = init_F3(iRandomf(), iRandomf(), 0.f);
        if (rangeCheck(eval_CombinedNoise(d, x_F3(pos), y_F3(pos)))) {
            return pos;
        }
    }
    return init1_F3(-1.f);
}

void setOffset_CombinedNoise(iCombinedNoise *d, size_t index, float offset) {
    ((iCombinedNoisePart *) at_Array(&d->parts, index))->offset = offset;
}
