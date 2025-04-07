#ifndef BOAT_H
#define BOAT_H

#define MAXBOATS 120
#define MAXNAMELENGTH 128

//----LocationType identifies where the boat is stored
typedef enum {
  SLIP,
  LAND,
  TRAILOR,
  STORAGE
} LocationType;

//----LocationDetail stores extra info for each location type
typedef union {
  int slipNumber;
  char bayLetter;
  char trailorTag[16];
  int storageNumber;
} LocationDetail;

//----Boat holds full information about each boat in the marina
typedef struct {
  char name[MAXNAMELENGTH];
  float length;
  LocationType locationType;
  LocationDetail locationDetail;
  float amountOwed;
} Boat;

#endif

