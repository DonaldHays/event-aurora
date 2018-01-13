#include "particles.h"
#include "../sprites.h"
#include "../rand.h"

#pragma bank 4

// ===
// Private Defines
// ===
typedef struct {
    GBUInt8 x;
    GBUInt8 y;
    GBUInt8 isActive;
    GBUInt8 age;
    GBUInt8 type;
    GBUInt8 attributes;
    GBUInt8 tileIndex;
    GBUInt8 user0;
} Particle;

typedef enum {
    particleTypeSmoke
} ParticleType;

#define particlesCount 8

// ===
// Private Variables
// ===
Particle _particles[particlesCount];
GBUInt8 _particlesCycleCounter;

// ===
// Private API
// ===
GBUInt8 _particlesFindIndex() {
    GBUInt8 index;
    
    for(index = 0; index != particlesCount; index++) {
        if(_particles[index].isActive == false) {
            return index;
        }
    }
    
    return 0;
}

// ===
// Public API
// ===
void particlesReset() {
    GBUInt8 index;
    
    _particlesCycleCounter = 0;
    
    for(index = 0; index != particlesCount; index++) {
        _particles[index].isActive = false;
    }
}

void particlesUpdate() {
    Particle * particle;
    GBUInt8 index;
    GBUInt8 * spriteAttributesPointer;
    
    _particlesCycleCounter = (_particlesCycleCounter + 1) & 0x01;
    
    for(index = 0; index != particlesCount; index++) {
        particle = &_particles[index];
        
        if(particle->isActive == false) {
            continue;
        }
        
        particle->age++;
        
        if(particle->type == particleTypeSmoke) {
            if(particle->age == 10) {
                particle->tileIndex = 241;
            } else if(particle->age == 17) {
                particle->tileIndex = 242;
            } else if(particle->age == 25) {
                particle->isActive = false;
                continue;
            }
            
            if (((particle->age & 7) == 7)) {
                particle->y -= 1;
            }
            
            if(particle->age < 5 && (particle->age & 1)) {
                particle->x += (GBInt8)particle->user0;
            } else if (particle->age < 15 && ((particle->age & 3) == 3)) {
                particle->x += (GBInt8)particle->user0;
            }
        }
    }
    
    for(index = 0; index != (particlesCount / 2); index++) {
        particle = &_particles[index * 2 + _particlesCycleCounter];
        spriteAttributesPointer = (GBUInt8 *)(&spriteAttributes[36 + index]);
        
        if(particle->isActive == false) {
            *(spriteAttributesPointer++) = 0;
            *(spriteAttributesPointer) = 0;
        } else {
            // Y, X, Tile Index, Attributes
            *(spriteAttributesPointer++) = particle->y;
            *(spriteAttributesPointer++) = particle->x;
            *(spriteAttributesPointer++) = particle->tileIndex;
            *(spriteAttributesPointer) = particle->attributes;
        }
    }
}

void particlesSpawnSmoke(GBUInt8 x, GBUInt8 y, GBInt8 velocityX) {
    Particle * particle;
    
    (void)velocityX;
    
    particle = &(_particles[_particlesFindIndex()]);
    
    particle->x = x;
    particle->y = y;
    particle->isActive = true;
    particle->age = 0;
    particle->type = particleTypeSmoke;
    particle->attributes = spriteAttributesMake(random() & 0x01, random() & 0x01, 0);
    particle->tileIndex = 240;
    particle->user0 = velocityX;
}
