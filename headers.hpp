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

#endif