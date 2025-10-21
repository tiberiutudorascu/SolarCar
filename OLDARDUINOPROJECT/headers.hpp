#ifndef HEADERS_H
#define HEADERS_H

// Enumerare pentru coduri de eroare folosite in sistem
enum ERORI
{
    NICIO_EROARE,       // Valoare implicita: nu exista eroare
    EROARE_PRIMIRE,     // Eroare la receptia pe CAN
    EROARE_TRIMITERE,   // Eroare la trimiterea pe CAN
    EROARE_FILTRU,      // Eroare legata de configurarea filtrului CAN
    ALTA_EROARE,        // Eroare generica/necategorizata
}; 
extern ERORI bug;        // Variabila globala pentru ultima eroare observata

// Prototipuri: raportare eroare si utilitare de conversie Big-Endian 16-bit
void problem(ERORI eroare);
static inline int16_t be16s(const uint8_t *p);  // 2 bytes BE -> int16_t (cu semn)
static inline uint16_t be16u(const uint8_t *p); // 2 bytes BE -> uint16_t (fara semn)

// Clasa BMS: retine meta-informatii despre pachetul de baterii si parsari de mesaje CAN
class BMS
{
private:
    // Parametri configurabili ai pachetului
    uint8_t numCells;         // Numarul de celule
    float minCellVoltage;     // Tensiune minima per celula [V]
    float maxCellVoltage;     // Tensiune maxima per celula [V]
    float maxChargeCurrent;   // Curent maxim de incarcare [A]
    float minChargeCurrent;   // Curent minim de incarcare [A] (poate fi 0 sau negativ pentru descarcare limit)
    float minChargeTemp;      // Temperatura minima la incarcare [C]
    float maxChargeTemp;      // Temperatura maxima la incarcare [C]
    float maxDischargeTemp;   // Temperatura maxima la descarcare [C]
    char cellType[16];        // Tipul/chemistry-ul celulei (ex: Li-ion)
    float capacityWh;         // Capacitate nominala a pachetului [Wh]

    // Factori de scalare pentru interpretarea payload-urilor CAN
    static constexpr float VOLT_CURR_SCALE = 0.1f;     // 1 LSB = 0.1 V
    static constexpr float SOC_SCALE = 0.5f;      // 1 LSB = 0.5 %
    static constexpr float SOH_TEMP_SCALE = 1.0f; // 1 LSB = 1 % sau 1 grad C in unele frame-uri
    static constexpr float FAN_V_SCALE = 0.01f;   // 1 LSB = 0.01 V

public:
    // NOTA: Comentariu autor - clasa ar putea deveni template pentru a pastra local datele si a decide in VCU.
    // In acest context PACKVOLTAGE, CURRENT si SOC ar fi campuri cheie prelucrate local.

    // Structura de date operata de parserele CAN ale BMS
    typedef struct __attribute__((packed))
    {
        // Campuri "largi" (comentariile originale mentioneaza 2 bytes, dar tipurile sunt float)
        // Asadar aici se stocheaza valorile deja scalate in unitati fizice, nu brute din CAN.
        float BMSpackVoltage;     // Tensiunea pachetului  [V]
        float BMSpackCurrent;     // Curentul pachetului [mA]
        int8_t BMSMaximumPackVoltage; //Tensiunea maxima a pachetului [V]
        int16_t BMSFlags;        // Flaguri 
        int8_t BMSPackResistance; // [miliOhm] cred ca acolo scrie mOhm
        int8_t BMSInternalTemperature; //Temperatura heatsinkului [C]

        // Campuri "de 1 byte" in payload, dar aici pastrate ca tipuri scalate/semnate
        float  BMSsoc;                // State of Charge [%]
        int8_t BMSsoh;                // State of Health [%] 
        int8_t BMSlowestThermistorID; // ID termistor cu temperatura minima
        int8_t BMShighestThermistorID;// ID termistor cu temperatura maxima
        int8_t BMShighestTemperature; // Temperatura maxima [C]
        int8_t BMSlowestTemperature;  // Temperatura minima [C]
        int8_t BMSaverageTemperature; // Temperatura medie [C]
        int8_t BMSfanSpeed;           // Treapta ventilator (0-6)
        float  BMSfanVoltage;          // Tensiune alimentare ventilator [V]
    } DATA_t;

    // Parsere pentru mesaje CAN specifice (completeaza structura DATA_t)
    void PACK_DATA(const CAN_message_t &rx, DATA_t &BMSDATA); // extrage V, I, SOC, SOH
    void THERM_DATA(const CAN_message_t &rx, DATA_t &BMSDATA);   // extrage temperaturi si ID-uri termistori
    void FAN_DATA(const CAN_message_t &rx, DATA_t &BMSDATA);   // extrage viteza si tensiunea ventilatorului
    void PACK_STATUS(const CAN_message_t &rx, DATA_t &BMSDATA,LimitReasons &CLS); //status mai general pentru BMS

    // Constructor cu toti parametrii principali ai pachetului
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
        // Copiere sigura a tipului de celula in bufferul fix
        strncpy(cellType, cellType_param, sizeof(cellType) - 1);
        cellType[sizeof(cellType) - 1] = '\0';
    }
    ~BMS() {} //Deconstructorul

    // Getters pentru acces read-only la atributele private
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

static constexpr uint16_t DCL_LOW_SOC                 = (1u << 0);
static constexpr uint16_t DCL_HIGH_RES                = (1u << 1);
static constexpr uint16_t DCL_TEMP                    = (1u << 2);
static constexpr uint16_t DCL_LOW_CELL_V              = (1u << 3);
static constexpr uint16_t DCL_LOW_PACK_V              = (1u << 4);
static constexpr uint16_t DCL_CCL_V_FAILSAFE          = (1u << 6);
static constexpr uint16_t DCL_CCL_COMM_FAILSAFE       = (1u << 7);

static constexpr uint16_t CCL_HIGH_SOC                = (1u << 9);
static constexpr uint16_t CCL_HIGH_RES                = (1u << 10);
static constexpr uint16_t CCL_TEMP                    = (1u << 11);
static constexpr uint16_t CCL_HIGH_CELL_V             = (1u << 12);
static constexpr uint16_t CCL_HIGH_PACK_V             = (1u << 13);
static constexpr uint16_t CCL_CHARGER_LATCH           = (1u << 14);
static constexpr uint16_t CCL_ALT_CURRENT_LIMIT_MPI   = (1u << 15);

struct LimitReasons {
  bool dcl_low_soc;
  bool dcl_high_res;
  bool dcl_temp;
  bool dcl_low_cell_v;
  bool dcl_low_pack_v;
  bool dcl_ccl_voltage_failsafe;
  bool dcl_ccl_comm_failsafe;

  bool ccl_high_soc;
  bool ccl_high_res;
  bool ccl_temp;
  bool ccl_high_cell_v;
  bool ccl_high_pack_v;
  bool ccl_charger_latch;
  bool ccl_alt_current_limit_mpi;
};

inline LimitReasons decode_limit_flags(uint16_t flags)
{
  LimitReasons r{};
  r.dcl_low_soc               = (flags & DCL_LOW_SOC);
  r.dcl_high_res              = (flags & DCL_HIGH_RES);
  r.dcl_temp                  = (flags & DCL_TEMP);
  r.dcl_low_cell_v            = (flags & DCL_LOW_CELL_V);
  r.dcl_low_pack_v            = (flags & DCL_LOW_PACK_V);
  r.dcl_ccl_voltage_failsafe  = (flags & DCL_CCL_V_FAILSAFE);
  r.dcl_ccl_comm_failsafe     = (flags & DCL_CCL_COMM_FAILSAFE);

  r.ccl_high_soc              = (flags & CCL_HIGH_SOC);
  r.ccl_high_res              = (flags & CCL_HIGH_RES);
  r.ccl_temp                  = (flags & CCL_TEMP);
  r.ccl_high_cell_v           = (flags & CCL_HIGH_CELL_V);
  r.ccl_high_pack_v           = (flags & CCL_HIGH_PACK_V);
  r.ccl_charger_latch         = (flags & CCL_CHARGER_LATCH);
  r.ccl_alt_current_limit_mpi = (flags & CCL_ALT_CURRENT_LIMIT_MPI);
  return r;
}
