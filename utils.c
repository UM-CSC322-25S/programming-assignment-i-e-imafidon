#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/*
  loadData

  Loads the boats from a CSV file. Skips invalid lines. Each line
  must match the format: name,length,type,extra,amount
*/
int loadData(const char *filename, Boat *boats[]) {
  FILE *file;
  char line[256];
  int count;
  file = fopen(filename, "r");
  count = 0;

  if (file == NULL) {
    return -1;
  }

  while (fgets(line, sizeof(line), file) != NULL && count < MAXBOATS) {
    Boat *b;
    char type[16];
    char extra[32];

    b = malloc(sizeof(Boat));
    if (b == NULL) {
      break;
    }

    //---- Skip blank or malformed lines
    if (strlen(line) < 5 || strchr(line, ',') == NULL) {
      free(b);
      continue;
    }

    line[strcspn(line, "\n")] = '\0';

    if (sscanf(line, "%127[^,],%f,%15[^,],%31[^,],%f",
               b->name, &b->length, type, extra, &b->amountOwed) != 5) {
      free(b);
      continue;
    }

    if (strcmp(type, "slip") == 0) {
      b->locationType = SLIP;
      b->locationDetail.slipNumber = atoi(extra);
    } else if (strcmp(type, "land") == 0) {
      b->locationType = LAND;
      b->locationDetail.bayLetter = extra[0];
    } else if (strcmp(type, "trailor") == 0) {
      b->locationType = TRAILOR;
      strncpy(b->locationDetail.trailorTag, extra, 15);
      b->locationDetail.trailorTag[15] = '\0';
    } else if (strcmp(type, "storage") == 0) {
      b->locationType = STORAGE;
      b->locationDetail.storageNumber = atoi(extra);
    } else {
      free(b);
      continue;
    }

    boats[count] = b;
    count++;
  }

  fclose(file);
  return count;
}

/*
  saveData

  Saves the boats back to the CSV file with the same format.
*/
int saveData(const char *filename, Boat *boats[], int boatCount) {
  FILE *file;
  int i;
  file = fopen(filename, "w");

  if (file == NULL) {
    return -1;
  }

  for (i = 0; i < boatCount; i++) {
    Boat *b = boats[i];

    if (b->locationType == SLIP) {
      fprintf(file, "%s,%.0f,slip,%d,%.2f\n",
              b->name, b->length, b->locationDetail.slipNumber, b->amountOwed);
    } else if (b->locationType == LAND) {
      fprintf(file, "%s,%.0f,land,%c,%.2f\n",
              b->name, b->length, b->locationDetail.bayLetter, b->amountOwed);
    } else if (b->locationType == TRAILOR) {
      fprintf(file, "%s,%.0f,trailor,%s,%.2f\n",
              b->name, b->length, b->locationDetail.trailorTag, b->amountOwed);
    } else if (b->locationType == STORAGE) {
      fprintf(file, "%s,%.0f,storage,%d,%.2f\n",
              b->name, b->length, b->locationDetail.storageNumber, b->amountOwed);
    }
  }

  fclose(file);
  return 0;
}

/*
  compareBoats

  Used by qsort to sort boats alphabetically
*/
int compareBoats(const void *a, const void *b) {
  Boat *b1 = *(Boat **)a;
  Boat *b2 = *(Boat **)b;
  return strcasecmp(b1->name, b2->name);
}

/*
  printInventory

  Displays all boats in alphabetical order
*/
void printInventory(Boat *boats[], int boatCount) {
  int i;
  qsort(boats, boatCount, sizeof(Boat *), compareBoats);

  for (i = 0; i < boatCount; i++) {
    Boat *b = boats[i];

    printf("%-20s %4.0f' ", b->name, b->length);

    if (b->locationType == SLIP) {
      printf("   slip   # %2d", b->locationDetail.slipNumber);
    } else if (b->locationType == LAND) {
      printf("   land     %c ", b->locationDetail.bayLetter);
    } else if (b->locationType == TRAILOR) {
      printf("trailor %s", b->locationDetail.trailorTag);
    } else if (b->locationType == STORAGE) {
      printf("storage  # %2d", b->locationDetail.storageNumber);
    }

    printf("   Owes $%7.2f\n", b->amountOwed);
  }
}

/*
  findBoatByName

  Searches for a boat by name (case-insensitive)
*/
int findBoatByName(Boat *boats[], int boatCount, const char *name) {
  int i;
  for (i = 0; i < boatCount; i++) {
    if (strcasecmp(boats[i]->name, name) == 0) {
      return i;
    }
  }
  return -1;
}

/*
  addBoatFromCSV

  Adds a new boat by parsing a CSV-formatted string
*/
int addBoatFromCSV(char *line, Boat *boats[], int boatCount) {
  Boat *b;
  char type[16];
  char extra[32];

  if (boatCount >= MAXBOATS) {
    return boatCount;
  }

  b = malloc(sizeof(Boat));
  if (b == NULL) {
    return boatCount;
  }

  if (sscanf(line, "%127[^,],%f,%15[^,],%31[^,],%f",
             b->name, &b->length, type, extra, &b->amountOwed) != 5) {
    free(b);
    return boatCount;
  }

  if (strcmp(type, "slip") == 0) {
    b->locationType = SLIP;
    b->locationDetail.slipNumber = atoi(extra);
  } else if (strcmp(type, "land") == 0) {
    b->locationType = LAND;
    b->locationDetail.bayLetter = extra[0];
  } else if (strcmp(type, "trailor") == 0) {
    b->locationType = TRAILOR;
    strncpy(b->locationDetail.trailorTag, extra, 15);
    b->locationDetail.trailorTag[15] = '\0';
  } else if (strcmp(type, "storage") == 0) {
    b->locationType = STORAGE;
    b->locationDetail.storageNumber = atoi(extra);
  } else {
    free(b);
    return boatCount;
  }

  boats[boatCount] = b;
  return boatCount + 1;
}

/*
  removeBoat

  Removes a boat from the list by name
*/
int removeBoat(Boat *boats[], int *boatCount, const char *name) {
  int index;
  int i;
  index = findBoatByName(boats, *boatCount, name);

  if (index == -1) {
    return 0;
  }

  free(boats[index]);

  for (i = index; i < *boatCount - 1; i++) {
    boats[i] = boats[i + 1];
  }

  (*boatCount)--;
  return 1;
}

/*
  acceptPayment

  Applies payment toward amount owed
*/
int acceptPayment(Boat *boats[], int boatCount, const char *name, float amount) {
  int index;
  index = findBoatByName(boats, boatCount, name);

  if (index == -1) {
    return -1;
  }

  if (amount > boats[index]->amountOwed) {
    return 0;
  }

  boats[index]->amountOwed -= amount;
  return 1;
}

/*
  chargeMonthlyFees

  Adds monthly charges based on location type and boat length
*/
void chargeMonthlyFees(Boat *boats[], int boatCount) {
  int i;
  for (i = 0; i < boatCount; i++) {
    Boat *b = boats[i];
    if (b->locationType == SLIP) {
      b->amountOwed += b->length * 12.5;
    } else if (b->locationType == LAND) {
      b->amountOwed += b->length * 14.0;
    } else if (b->locationType == TRAILOR) {
      b->amountOwed += b->length * 25.0;
    } else if (b->locationType == STORAGE) {
      b->amountOwed += b->length * 11.2;
    }
  }
}

/*
  freeAllBoats

  Frees all dynamically allocated boat records
*/
void freeAllBoats(Boat *boats[], int boatCount) {
  int i;
  for (i = 0; i < boatCount; i++) {
    free(boats[i]);
  }
}

