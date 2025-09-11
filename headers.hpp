#ifndef HEADERS_H
#define HEADERS_H
enum ERORI
{
    NICIO_EROARE,
    EROARE_PRIMIRE,
    EROARE_TRIMITERE,
    EROARE_FILTRU,
    ALTA_EROARE,
};
extern ERORI bug;

void problem(ERORI eroare);
static inline int16_t be16s(const uint8_t *p);
static inline uint16_t be16u(const uint8_t *p);

class BMS
{
private:
    uint8_t numCells;
    float minCellVoltage;
    float maxCellVoltage;
    float maxChargeCurrent;
    float minChargeCurrent;
    float minChargeTemp;
    float maxChargeTemp;
    float maxDischargeTemp;
    char cellType[16];
    float capacityWh;

    static constexpr float VOLT_SCALE = 0.1f;     // 1 LSB = 0.1 V
    static constexpr float CURR_SCALE = 0.1f;     // 1 LSB = 0.1 A
    static constexpr float SOC_SCALE = 0.5f;      // 1 LSB = 0.5 %
    static constexpr float SOH_TEMP_SCALE = 1.0f; // 1 LSB = 1 %

public:
    typedef struct
    {
        float BMSpackVoltage;
        float BMSpackCurrent;
        float BMSsoc;
        float BMSsoh;
        int8_t BMSlowestThermistorID = 50;
        int8_t BMShighestThermistorID = -50;
        int8_t BMShighestTemperature;
        int8_t BMSlowestTemperature;
        int8_t BMSfanSpeed, BMSfanVoltage;
        int8_t BMSaverageTemperature;

    } DATA_t;

    void PACK_DATA(const CAN_message_t &rx, DATA_t &BMSDATA);
    void THERMID(const CAN_message_t &rx, DATA_t &BMSDATA);

    BMS(uint8_t numCells_param,
        float minCellVoltage_param,
        float maxCellVoltage_param,
        float maxChargeCurrent_param,
        float minChargeCurrent_param,
        float minChargeTemp_param,
        float maxChargeTemp_param,
        float maxDischargeTemp_param,
        const char *cellType_param,
        float capacityWh_param)
        : numCells(numCells_param),
          minCellVoltage(minCellVoltage_param),
          maxCellVoltage(maxCellVoltage_param),
          maxChargeCurrent(maxChargeCurrent_param),
          minChargeCurrent(minChargeCurrent_param),
          minChargeTemp(minChargeTemp_param),
          maxChargeTemp(maxChargeTemp_param),
          maxDischargeTemp(maxDischargeTemp_param),
          capacityWh(capacityWh_param)
    {
        strncpy(cellType, cellType_param, sizeof(cellType) - 1);
        cellType[sizeof(cellType) - 1] = '\0';
    }
    ~BMS() {}

    // Getters
    uint8_t getNumCells() const;
    float getMinCellVoltage() const;
    float getMaxCellVoltage() const;
    float getMaxChargeCurrent() const;
    float getMinChargeCurrent() const;
    float getMinChargeTemp() const;
    float getMaxChargeTemp() const;
    float getMaxDischargeTemp() const;
    const char *getCellType() const;
    float getCapacityWh() const;
};
#endif