

#define DISCHARGE_POTENCIAL (5000.0f)

#define LEARNING_COEF (5.0f)

#define SCREEN_X (160)
#define SCREEN_Y (120)
#define SCREEN_SIZE (SCREEN_X*SCREEN_Y)

#define DATA_X (8)
#define DATA_Y (6)
#define DATA_SIZE (DATA_X*DATA_Y)

#define NEURON_S_ZONE_X (SCREEN_X/DATA_X)
#define NEURON_S_ZONE_Y (SCREEN_Y/DATA_Y)

#define R_NEURON_NUMBER (2)
#define S_NEURON_NUMBER DATA_SIZE
#define SYNAPSES_NUMBER (R_NEURON_NUMBER * S_NEURON_NUMBER)
#define NEURON_REFRACTORY_PERIOD (2)

#define POTENCIAL_MIN (0.0f)
#define POTENCIAL_DECAY_RATE (0.5f)

#define WEIGHT_MIN (-5000.0f)
#define WEIGHT_MAX (5000.0f)
#define WEIGHT_DEFAULT (500.0f)

#define TICKS_IN_CADRE (32)
#define SPIKES_ARRAY_TYPE unsigned long
#define ANSWER_TYPE float

#define TONE_DIFFERENCE (50)

#define FRAM_SCREEN_MATRIX_ADR 0x00
#define FRAM_EVENT_LIST_ADR (SCREEN_SIZE + 0x10)
#define EEPROM_WEIGHTS_ADR 0x01

#define TRIANGULAR_DESTRIBUTION_BEGIN (1)
#define TRIANGULAR_DESTRIBUTION_END TICKS_IN_CADRE
#define TRIANGULAR_DESTRIBUTION_LENGTH (8)
#define TRIANGULAR_DESTRIBUTION_PEAK_SHARE (DATA_SIZE/(TRIANGULAR_DESTRIBUTION_END-TRIANGULAR_DESTRIBUTION_BEGIN+1-TRIANGULAR_DESTRIBUTION_LENGTH))
#define TRIANGULAR_DESTRIBUTION_PEAK_CENTER (TRIANGULAR_DESTRIBUTION_BEGIN+TRIANGULAR_DESTRIBUTION_LENGTH/2)

#define CAMERA_WHITE_LIMIT (50)

float r_neurons_potencial[R_NEURON_NUMBER];

float synapses_weights[SYNAPSES_NUMBER];
char s_events_number[S_NEURON_NUMBER];

SPIKES_ARRAY_TYPE spikes_s[S_NEURON_NUMBER];
SPIKES_ARRAY_TYPE spikes_r[R_NEURON_NUMBER];
char refractery_timer[R_NEURON_NUMBER];

ANSWER_TYPE Transform_spikes_to_answer();
inline void Save_weights();
inline void Load_weights();
void R_potencial(unsigned char r_neuron_index, SPIKES_ARRAY_TYPE current_tick, bool& is_spike);
ANSWER_TYPE Transform_spikes_to_answer();
float Triangular_destribution(short a, short b, short c, short x);
void SNN_process();
