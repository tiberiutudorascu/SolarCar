#ifndef HEADERS_H
#define HEADERS_H

enum ERORI
{
    NICIO_EROARE,
    EROARE_PRIMIRE,
    EROARE_TRIMITERE,
    EROARE_FILTRU,
    ALTA_EROAROE,
};
ERORI bug = NICIO_EROARE;

void problem(ERORI eroare);
void PACK_VI_SOCH(const CAN_message_t &rx);
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
    static constexpr float VOLT_SCALE = 0.1; // 1 LSB = 0.1 V
    static constexpr float CURR_SCALE = 0.1; // 1 LSB = 0.1 A
    static constexpr float SOC_SCALE = 0.5;  // 1 LSB = 0.5 %
    static constexpr float SOH_SCALE = 1.0;  // 1 LSB = 1 %

public:
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
    void PACK_DATA(const CAN_message_t &rx);
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