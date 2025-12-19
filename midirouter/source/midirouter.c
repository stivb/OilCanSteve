/*
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
  const LV2_Atom_Sequence* midi_in;
  LV2_Atom_Sequence*       midi_out[7];
  const float* gatesBits;
  } ports;

  bool forwards[7];
  bool pathNeedsResetting[7];
  int currGatesBits;

  // URIs
  MidirouterURIs uris;
} Midirouter;

static void
connect_port(LV2_Handle instance, uint32_t port, void* data)
{
  Midirouter* self = (Midirouter*)instance;
 
  if (port==0)
  {
    self->ports.midi_in = (const LV2_Atom_Sequence*)data;
    return;
  }
  if (port<8 && port>0)
  {
    self->ports.midi_out[port-1] = (LV2_Atom_Sequence*)data;
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
  Midirouter* self = (Midirouter*)calloc(1, sizeof(Midirouter));
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
    lv2_log_error(&self->logger, "Missing feature <%s>\n", missing);
    free(self);
    return NULL;
  }

  for (int i=0;i<7;i++) self->forwards[i] = false;
  
  self->currGatesBits =  0;
  

  map_mididrums_uris(self->map, &self->uris);

  return (LV2_Handle)self;
}

static void
cleanup(LV2_Handle instance)
{
  free(instance);
}

void parseForwards (Midirouter* self);

void parseForwards (Midirouter* self)
{
  //for (int i = 0; i < 7; i++) self->forwards[i] = false;
  int workingVal = (int) *self->ports.gatesBits;
  int lastVal = self->currGatesBits;
  for (int i=0;i<7;i++) 
  {
    //lv2_log_note(&self->logger, "Doing %d\n", i);
    self->pathNeedsResetting[i] = false;
    if (workingVal & 1) self->forwards[i] = true;
    else 
    {
      self->forwards[i] = false;
      if (lastVal & 1) {
        self->pathNeedsResetting[i] = true;
        //lv2_log_note(&self->logger, "Tap at index %d has been closed\n", i);
      }

    }
    workingVal = workingVal >> 1;
    lastVal = lastVal >> 1;
  }
  self->currGatesBits = (int) *self->ports.gatesBits;

}

void resetPath(Midirouter* self, int pathIndex, uint32_t out_capacity);

void resetPath(Midirouter* self, int pathIndex, uint32_t out_capacity)
{
  MidirouterURIs* uris = &self->uris;
// Send All Notes Off and Pitch Bend to center for all 16 MIDI channels
  for (int ch = 0; ch < 16; ++ch) {
    struct { LV2_Atom_Event ev; uint8_t msg[3]; } evt;
    evt.ev.time.frames = 0;
    evt.ev.body.type = uris->midi_Event;
    evt.ev.body.size = 3;
    evt.msg[0] = 0xB0 | (uint8_t)ch; /* CC status for channel ch (All Notes Off) */
    evt.msg[1] = 123;               /* All Notes Off */
    evt.msg[2] = 0;
    lv2_atom_sequence_append_event(self->ports.midi_out[pathIndex], out_capacity, (LV2_Atom_Event*)&evt);
    struct { LV2_Atom_Event ev; uint8_t msg[3]; } pb;
    pb.ev.time.frames = 0;
    pb.ev.body.type = uris->midi_Event;
    pb.ev.body.size = 3;
    pb.msg[0] = 0xE0 | (uint8_t)ch; /* Pitch Bend status for channel ch */
    pb.msg[1] = 0x00;              /* LSB */
    pb.msg[2] = 0x40;              /* MSB (center) */
    lv2_atom_sequence_append_event(self->ports.midi_out[pathIndex], out_capacity, (LV2_Atom_Event*)&pb);
  }

}

  static void
run(LV2_Handle instance, uint32_t sample_count)
{
  Midirouter*     self = (Midirouter*)instance;
  MidirouterURIs* uris = &self->uris;

  // Get the capacity
  const uint32_t out_capacity = self->ports.midi_out[0]->atom.size;

  for (int i=0;i<7;i++)  {    
    lv2_atom_sequence_clear(self->ports.midi_out[i]);
    self->ports.midi_out[i]->atom.type = self->ports.midi_in->atom.type;  
  }


  if (self->currGatesBits != (int) *self->ports.gatesBits)
  {
    parseForwards(self);
    for (int i = 0; i < 7; ++i) {
      if (self->pathNeedsResetting[i]) {
        resetPath(self, i, out_capacity);
        self->pathNeedsResetting[i] = false;
      }
    }
   
  }



  
  
  // Read incoming events
  LV2_ATOM_SEQUENCE_FOREACH (self->ports.midi_in, ev) {
    if (ev->body.type == uris->midi_Event) {
      const uint8_t* const msg = (const uint8_t*)(ev + 1);
      switch (lv2_midi_message_type(msg)) {
      case LV2_MIDI_MSG_NOTE_ON:
      case LV2_MIDI_MSG_NOTE_OFF:
      case LV2_MIDI_MSG_BENDER:
        /* Fall through to forwarding (treat pitch bend like note messages) */
        
        for( int i = 0; i < 7; i++)
        {
          if (self->forwards[i])
          {
            lv2_atom_sequence_append_event(self->ports.midi_out[i], out_capacity, ev);
          }
        }
        
        break;
      default:
        // Forward all other MIDI events directly
        //lv2_atom_sequence_append_event(self->out_port, out_capacity, ev);
        break;
      }
    }
  }
}



static const void*
extension_data(const char* uri)
{
  return NULL;
}

static const LV2_Descriptor descriptor = {MIDIROUTER_URI,
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