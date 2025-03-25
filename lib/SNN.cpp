#include <Arduino.h>
#include "SNN.h"
#include "FRAM.h"
#include <EEPROM.h>

inline void Save_weights(){
  unsigned short adr = EEPROM_WEIGHTS_ADR;
  for (int i=0;i<SYNAPSES_NUMBER;i++){
    EEPROM.update(adr, synapses_weights[i]);
    adr+=sizeof(float);
  }
  return;
}

inline void Load_weights(){
  unsigned short adr = EEPROM_WEIGHTS_ADR;
  for (int i=0;i<SYNAPSES_NUMBER;i++){
    synapses_weights[i] = EEPROM.read(adr);
  }
  return;
}

void R_potencial(unsigned char r_neuron_index, SPIKES_ARRAY_TYPE current_tick, bool& is_spike) {
    float potencial = r_neurons_potencial[r_neuron_index];
    unsigned char i = 0;

    if (refractery_timer[r_neuron_index]) {
        refractery_timer[r_neuron_index]--;
        r_neurons_potencial[r_neuron_index] = POTENCIAL_MIN;
        return;
    }

    for (; i < S_NEURON_NUMBER; i++) {
        if (current_tick && spikes_s[i]) {
            potencial += synapses_weights[i * R_NEURON_NUMBER + r_neuron_index];
        }
    }

    potencial -= (1.0f-POTENCIAL_DECAY_RATE) * (r_neurons_potencial[r_neuron_index]);

    if (potencial > DISCHARGE_POTENCIAL) {
        refractery_timer[r_neuron_index] = NEURON_REFRACTORY_PERIOD;
        is_spike = true;
        potencial = POTENCIAL_MIN;
    }

    r_neurons_potencial[r_neuron_index] = potencial;
}

ANSWER_TYPE Transform_spikes_to_answer() {
    unsigned short l = 0, r = 0;

    for (SPIKES_ARRAY_TYPE counter = 0x1; counter != 0; counter <<= 1) {
        if (spikes_r[0] & counter) {
            l++;
        }
        if (spikes_r[1] & counter) {
            r++;
        }
    }

    return (float)(r - l)*2 / TICKS_IN_CADRE;
}

float Triangular_destribution(short a, short b, short c, short x) {
    if (x<a || x>b) return 0;
    if (x < c) {
        return (float)2 * (x - a) / ((b - a) * (c - a));
    }
    return (float)2 * (b - x) / ((b - a) * (b - c));
}

void SNN_process() {
    for (int i = 0; i < S_NEURON_NUMBER; i++) {
      unsigned char events_number = Read_fram(i);
        float spike_rate = 1.0f * 16 * (events_number / (TICKS_IN_CADRE));
        for (int j = 0, counter = 0x1; j < TICKS_IN_CADRE; j++, counter <<= 1) {
            float random_value = (float)random(WINT_MAX) / (float)WINT_MAX;
            float distrib = Triangular_destribution(TRIANGULAR_DESTRIBUTION_BEGIN + (i / TRIANGULAR_DESTRIBUTION_PEAK_SHARE), TRIANGULAR_DESTRIBUTION_BEGIN + TRIANGULAR_DESTRIBUTION_LENGTH + (i / TRIANGULAR_DESTRIBUTION_PEAK_SHARE), TRIANGULAR_DESTRIBUTION_PEAK_CENTER + (i / TRIANGULAR_DESTRIBUTION_PEAK_SHARE), j + 1);
            random_value /= spike_rate;
            if (random_value <= distrib) {
                spikes_s[i] = spikes_s[i] | counter;
            }
            else {
                spikes_s[i] = spikes_s[i] & ~counter;
            }
        }
    }

    for (int i = 0, counter = 0x1; i < TICKS_IN_CADRE; i++, counter <<= 1) {
        for (int j = 0; j < R_NEURON_NUMBER; j++) {
            bool isSpike = false;
            R_potencial(j, counter, isSpike);
            if (isSpike) {
                spikes_r[j] = spikes_r[j] | counter;
            }
            else {
                spikes_r[j] = spikes_r[j] & ~counter;
            }
        }
    }
}