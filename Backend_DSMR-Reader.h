// **********************************
// * Backend DSMR-Reader            *
// **********************************

#ifndef BACKEND_DSMR_READER_H
#define BACKEND_DSMR_READER_H

#include "P1Reader.h"

void Send_to_DSMR_Reader(MyData data, MyData LastData, bool NewRejectedTelegram, double RejectedTelegrams);

#endif /* BACKEND_DSMR_READER_H */
