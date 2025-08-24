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
#endif