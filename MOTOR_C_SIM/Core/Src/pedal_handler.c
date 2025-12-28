#include "pedal_handler.h"
#include <stdlib.h>
#include <stdbool.h> // <--- ADAUGAT: Rezolva eroarea cu 'true'/'false'
#include "main.h"    // <--- ADAUGAT: Rezolva eroarea cu 'hadc1'

PedalHandler_t gPedal;

/* --- CALIBRARE --- */
#define ADC_MIN_VALID  100
#define ADC_MAX_VALID  4000
#define MAX_DEVIATION  400
#define FILTER_SIZE    10

/* Buffere circulare pentru medie */
static uint16_t buf1[FILTER_SIZE] = {0};
static uint16_t buf2[FILTER_SIZE] = {0};
static uint8_t idx = 0;

/* Functie interna: Citeste ADC schimband canalul dinamic */
static uint16_t ADC_Read(uint32_t Channel) {
    ADC_ChannelConfTypeDef sConfig = {0};
    uint16_t val = 0;

    sConfig.Channel = Channel;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

    // 'hadc1' este definita in main.h ca extern, acum o vedem
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) return 0;

    HAL_ADC_Start(&hadc1);
    if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK) {
        val = HAL_ADC_GetValue(&hadc1);
    }
    HAL_ADC_Stop(&hadc1);
    return val;
}

// ... restul functiilor (Apply_Filter, Pedal_Init, Pedal_Process) raman la fel ...
static uint16_t Apply_Filter(uint16_t new_val, uint16_t *buffer) {
    buffer[idx] = new_val;
    uint32_t sum = 0;
    for(int i=0; i<FILTER_SIZE; i++) sum += buffer[i];
    return (uint16_t)(sum / FILTER_SIZE);
}

void Pedal_Init(void) {
    gPedal.status = PEDAL_OK;
    gPedal.throttle_pct = 0;
    gPedal.safe_state_locked = false;
}

void Pedal_Process(void) {
    gPedal.raw_adc1 = ADC_Read(ADC_CHANNEL_0);
    gPedal.raw_adc2 = ADC_Read(ADC_CHANNEL_1);

    gPedal.filt_adc1 = Apply_Filter(gPedal.raw_adc1, buf1);
    gPedal.filt_adc2 = Apply_Filter(gPedal.raw_adc2, buf2);

    idx = (idx + 1) % FILTER_SIZE;

    PedalStatus_t current_error = PEDAL_OK;

    if (gPedal.filt_adc1 < ADC_MIN_VALID) current_error = ERR_POT1_DISCONNECTED;
    else if (gPedal.filt_adc2 < ADC_MIN_VALID) current_error = ERR_POT2_DISCONNECTED;
    else if (gPedal.filt_adc1 > ADC_MAX_VALID) current_error = ERR_POT1_SHORT;
    else if (abs((int)gPedal.filt_adc1 - (int)gPedal.filt_adc2) > MAX_DEVIATION) {
        current_error = ERR_IMPLAUSIBLE;
    }

    if (current_error != PEDAL_OK) {
        gPedal.safe_state_locked = true;
        gPedal.status = current_error;
    }

    if (gPedal.safe_state_locked) {
        gPedal.throttle_pct = 0;
        uint32_t avg = (gPedal.filt_adc1 + gPedal.filt_adc2) / 2;
        if (current_error == PEDAL_OK && avg < 300) {
            gPedal.safe_state_locked = false;
            gPedal.status = PEDAL_OK;
        }
    }
    else {
        uint32_t avg = (gPedal.filt_adc1 + gPedal.filt_adc2) / 2;
        if (avg > 4095) avg = 4095;
        gPedal.throttle_pct = (uint8_t)((avg * 100) / 4095);
    }
}
