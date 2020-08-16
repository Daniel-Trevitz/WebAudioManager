#ifndef DEVICE_INTERACTION_H
#define DEVICE_INTERACTION_H

#include "interface/channel.h"
#include "interface/player.h"
#include "interface/ripper.h"
#include "interface/volume.h"
#include "interface/radio.h"
#include "interface/library.h"
#include "interface/settings.h"

extern void *device_interaction(void *);

extern Settings *settings;
extern Player *player;
extern Volume *volume;
extern Channel *ch; // changing channels requires muting

extern int ledBrightness;

#endif // DEVICE_INTERACTION_H
