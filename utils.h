#ifndef UTILSH
#define UTILSH

#include "boat.h"

int loadData(const char *filename, Boat *boats[]);
int saveData(const char *filename, Boat *boats[], int boatCount);
void printInventory(Boat *boats[], int boatCount);
int addBoatFromCSV(char *line, Boat *boats[], int boatCount);
int findBoatByName(Boat *boats[], int boatCount, const char *name);
int removeBoat(Boat *boats[], int *boatCount, const char *name);
int acceptPayment(Boat *boats[], int boatCount, const char *name, float amount);
void chargeMonthlyFees(Boat *boats[], int boatCount);
void freeAllBoats(Boat *boats[], int boatCount);

#endif

