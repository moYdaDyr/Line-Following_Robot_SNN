

#include <iostream>
#include <windows.h>
#include <fstream>
#include <cmath>

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

#define TONE_DIFFERENCE (10)

#define TEST_ARRAY (800)
#define TRAIN_ARRAY (1800)

#define TRIANGULAR_DESTRIBUTION_BEGIN (1)
#define TRIANGULAR_DESTRIBUTION_END TICKS_IN_CADRE
#define TRIANGULAR_DESTRIBUTION_LENGTH (8)
#define TRIANGULAR_DESTRIBUTION_PEAK_SHARE (DATA_SIZE/(TRIANGULAR_DESTRIBUTION_END-TRIANGULAR_DESTRIBUTION_BEGIN+1-TRIANGULAR_DESTRIBUTION_LENGTH))
#define TRIANGULAR_DESTRIBUTION_PEAK_CENTER (TRIANGULAR_DESTRIBUTION_BEGIN+TRIANGULAR_DESTRIBUTION_LENGTH/2)

using namespace std;

short event_map[DATA_X][DATA_Y];

float r_neurons_potencial[R_NEURON_NUMBER];

float synapses_weights[SYNAPSES_NUMBER];
char s_events_number[S_NEURON_NUMBER];

SPIKES_ARRAY_TYPE spikes_s[S_NEURON_NUMBER];
SPIKES_ARRAY_TYPE spikes_r[R_NEURON_NUMBER];
char refractery_timer[R_NEURON_NUMBER];

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

    cout << "left " << dec << l << " right " << r << endl;

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
        float spike_rate = 1.0f * 16 * (event_map[i%DATA_X][i/DATA_X] / (TICKS_IN_CADRE));
        for (int j = 0, counter = 0x1; j < TICKS_IN_CADRE; j++, counter <<= 1) {
            float random_value = (float)rand() / (float)RAND_MAX;
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

    for (int i = 0; i < R_NEURON_NUMBER; i++) {
    }
}

float Get_correction(float answer, float right_answer) {
    if (right_answer >= 0 && answer >= 0 || right_answer <= 0 && answer <= 0) {
        return (right_answer - answer) * (right_answer - answer);
    }
}

float Get_error(float answer, float right_answer) {
    if (right_answer >= 0 && answer >= 0 || right_answer <= 0 && answer <= 0) {
        return (right_answer - answer) * (right_answer - answer);
    }
    
}

int main()
{
    fstream f;

    srand(time(0));

    for (int i = 0; i < SYNAPSES_NUMBER; i++) {
        synapses_weights[i] = WEIGHT_DEFAULT;
    }

    for (int i = 0; i < S_NEURON_NUMBER; i++) {
        spikes_s[i] = 0;
    }
    for (int i = 0; i < R_NEURON_NUMBER; i++) {
        spikes_r[i] = 0;
    }

    for (int e = 0;; e++) {

        float right_answer;
        float answer;
        float correction;
        float error_sum=0;

        f.open("C:\\Users\\potor\\Documents\\курсач_3\\train_set.dvs", ios::in);

        for (int w = 2; w <= TRAIN_ARRAY; w++) {

            

            int n = 0;

            for (int i = 0; i < S_NEURON_NUMBER; i++) {
                event_map[i%DATA_X][i/DATA_X] = 0;
            }

            f >> n;
            for (int i = 0; i < n; i++) {
                short x, y;
                bool is_positive;
                f >> x >> y >> is_positive;
                    event_map[x/NEURON_S_ZONE_X][y/NEURON_S_ZONE_Y]++;
            }

            SNN_process();

            answer = Transform_spikes_to_answer();

            f >> right_answer;

            cout << "answer: " << answer << " right answer: " << right_answer << endl;

            correction = Get_correction(answer,right_answer);
            if (right_answer > answer) {
                correction = -correction;
            }

            cout << "correction: " << correction << endl;

            // correction

            float delta_w_array[SYNAPSES_NUMBER] = { 0.0f };

            for (int i = 0; i < R_NEURON_NUMBER; i++) {
                for (SPIKES_ARRAY_TYPE counter_r = 0x1, timer_r=0; counter_r > 0; counter_r <<= 1, timer_r++) {
                    if (spikes_r[i] & counter_r) {
                        for (int j = 0; j < S_NEURON_NUMBER; j++) {
                            float delta_w = 0.0f;
                            for (SPIKES_ARRAY_TYPE counter_s = 0x1, timer_s=0; counter_s > 0; counter_s <<=1, timer_s++) {
                                if (spikes_s[j] & counter_s) {
                                    delta_w += correction * LEARNING_COEF * (TICKS_IN_CADRE/(timer_r - timer_s == 0 ? 1 : timer_r - timer_s));
                                }
                            }
                            delta_w_array[j * R_NEURON_NUMBER + i] = delta_w;
                        }
                    }
                }
                correction = -correction;
            }

            for (int i = 0; i < SYNAPSES_NUMBER; i++) {
                synapses_weights[i] = max(synapses_weights[i] + delta_w_array[i], WEIGHT_MIN);
            }

        }

        f.close();

        f.open("C:\\Users\\potor\\Documents\\курсач_3\\test_set.dvs", ios::in);

        for (int w = 1; w <= TEST_ARRAY; w++) {

            int n = 0;

            for (int i = 0; i < S_NEURON_NUMBER; i++) {
                event_map[i % DATA_X][i / DATA_X] = 0;
            }

            f >> n;
            for (int i = 0; i < n; i++) {
                short x, y;
                bool is_positive;
                f >> x >> y >> is_positive;
                event_map[x / NEURON_S_ZONE_X][y / NEURON_S_ZONE_Y]++;
            }

            SNN_process();

            answer = Transform_spikes_to_answer();

            f >> right_answer;

            correction = abs(Get_error(answer,right_answer));

            error_sum += correction;

            cout << "answer: " << answer << " right answer: " << right_answer << endl;
        }

        f.close();

        cout << "Epoch: " << e << " Error: " << error_sum;
        cout << "Show weights? (y/n)" << endl;

        char show_weights;
        cin >> show_weights;
        if (show_weights == 'y') {
            for (int i = 0; i < SYNAPSES_NUMBER; i++) {
                cout << synapses_weights[i] << " ";
            }
            cout << endl;
        }
        else if (show_weights != 'n') {
            return 0;
        }
    }

}


