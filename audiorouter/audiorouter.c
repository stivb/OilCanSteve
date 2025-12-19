/*
 * audiorouter - LV2 audio routing plugin
 * Copyright (C) 2024
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "./uris.h"

#include "lv2/atom/atom.h"
#include "lv2/atom/util.h"
#include "lv2/core/lv2.h"
#include "lv2/core/lv2_util.h"
#include "lv2/log/log.h"
#include "lv2/log/logger.h"
#include "lv2/midi/midi.h"
#include "lv2/urid/urid.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>




void debugMidiNote(LV2_Handle instance, LV2_Atom_Event * event, int8_t  msg[]);
 

typedef struct {
  // Features
  LV2_URID_Map*  map;
  LV2_Log_Logger logger;

  // Ports
  struct {
  const float*  audio_in;
  float*       audio_out[7];
  const float* gatesBits;
  } ports;

  bool forwards[7];
  int currMidiGatesHex;


  // URIs
  AudiorouterURIs uris;
} Audiorouter;



static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Audiorouter* self = (Audiorouter*)instance;
  // Log port connection only during debugging to avoid excessive logging

  //lv2_log_note(&self->logger, "Connecting  Port %d\n", port);
  

  if (port==0)
  {
    self->ports.audio_in = (const float*) data;;
    return;
  }
  if (port<8 && port>0)
  {
    self->ports.audio_out[port-1] = (float*) data;
    return;
  }
  if (port==8)
  {
    self->ports.gatesBits = (const float*)data;
    return;
  }
 }

   


static LV2_Handle
instantiate(const LV2_Descriptor*     descriptor,
            double                    rate,
            const char*               path,
            const LV2_Feature* const* features)
{
  // Allocate and initialise instance structure.
  Audiorouter* self = (Audiorouter*)calloc(1, sizeof(Audiorouter));
  if (!self) {
    return NULL;
  }

  // Scan host features for URID map
  // clang-format off
  const char*  missing = lv2_features_query(
    features,
    LV2_LOG__log,  &self->logger.log, false,
    LV2_URID__map, &self->map,        true,
    NULL);
  // clang-format on

  lv2_log_logger_set_map(&self->logger, self->map);
  if (missing) {
    //lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
    free(self);
    return NULL;
  }



  map_audiorouter_uris(self->map, &self->uris);

  for (int i=0;i<7;i++) self->forwards[i] = false;

  self->currMidiGatesHex =  0;

  return (LV2_Handle)self;
}

static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

void parseForwards (Audiorouter* self);

void parseForwards (Audiorouter* self)
{
  for (int i = 0; i < 7; i++) self->forwards[i] = false;
  int workingVal = (int) *self->ports.gatesBits;
  for (int i=0;i<7;i++) if (workingVal & (1<<i)) self->forwards[i] = true;

}

static void write_output(Audiorouter* self, uint32_t len);

static void
write_output(Audiorouter* self, uint32_t len)
{
  for (int i = 0; i < 7; i++)
  {
    if (self->forwards[i]) memcpy(self->ports.audio_out[i] , self->ports.audio_in , len * sizeof(float));
    else memset(self->ports.audio_out[i], 0, len * sizeof(float));
  }
}

static void
run(LV2_Handle instance, uint32_t sample_count)
{
  Audiorouter*     self = (Audiorouter*)instance;


  if (self->currMidiGatesHex != (int) *self->ports.gatesBits)
  {
    self->currMidiGatesHex = (int) *self->ports.gatesBits;
    parseForwards(self);
  }
    write_output(self, sample_count); 
}


static const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {AUDIOROUTER_URI,
                                          instantiate,
                                          connect_port,
                                          NULL, // activate,
                                          run,
                                          NULL, // deactivate,
                                          cleanup,
                                          extension_data};

LV2_SYMBOL_EXPORT
const LV2_Descriptor*
lv2_descriptor(uint32_t index)
{
  return index == 0 ? &descriptor : NULL;
}