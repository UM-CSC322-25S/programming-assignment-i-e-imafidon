#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/*
  Program BoatManagement

  This program manages a list of boats at a marina. It loads data from a CSV file,
  allows the user to view, add, remove, pay for, or update boats, and then saves
  the updated data back to the CSV file.
*/

int main(int argc, char *argv[]) {
  Boat *boats[MAXBOATS];
  int boatCount = 0;
  char option;
  char line[256];
  char filename[128];
  int result;

  if (argc != 2) {
    printf("Usage: %s datafile.csv\n", argv[0]);
    return 1;
  }

  strncpy(filename, argv[1], 127);
  filename[127] = '\0';

  boatCount = loadData(filename, boats);

  if (boatCount < 0) {
    printf("Could not load file %s\n", filename);
    return 1;
  }

  printf("Welcome to the Boat Management System\n");
  printf("-------------------------------------\n");

  while (1) {
    printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
    if (fgets(line, sizeof(line), stdin) == NULL) {
      continue;
    }

    option = tolower(line[0]);

    if (option == 'i') {
      printInventory(boats, boatCount);
    } else if (option == 'a') {
      printf("Please enter the boat data in CSV format                 : ");
      if (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        boatCount = addBoatFromCSV(line, boats, boatCount);
      }
    } else if (option == 'r') {
      printf("Please enter the boat name                               : ");
      if (fgets(line, sizeof(line), stdin) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        result = removeBoat(boats, &boatCount, line);
        if (result == 0) {
          printf("No boat with that name\n");
        }
      }
    } else if (option == 'p') {
      float amount;
      printf("Please enter the boat name                               : ");
      if (fgets(line, sizeof(line), stdin) == NULL) {
        continue;
      }

      line[strcspn(line, "\n")] = '\0';
      result = findBoatByName(boats, boatCount, line);

      if (result == -1) {
        printf("No boat with that name\n");
        continue;
      }

      printf("Please enter the amount to be paid                       : ");
      if (fgets(line, sizeof(line), stdin) != NULL) {
        sscanf(line, "%f", &amount);
        if (amount > boats[result]->amountOwed) {
          printf("That is more than the amount owed, $%.2f\n",
                 boats[result]->amountOwed);
        } else {
          boats[result]->amountOwed -= amount;
        }
      }
    } else if (option == 'm') {
      chargeMonthlyFees(boats, boatCount);
    } else if (option == 'x') {
      printf("Exiting the Boat Management System\n");
      break;
    } else {
      printf("Invalid option %c\n", line[0]);
    }
  }

  saveData(filename, boats, boatCount);
  freeAllBoats(boats, boatCount);
  return 0;
}

