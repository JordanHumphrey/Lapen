#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>
#include "Bunny.h"

static const char* Sex[2] = { "Male", "Female" };
static const char* Color[4] = { "Red", "Green", "Blue", "Purple" };
static const char* Names[11] = {
    "Roger", "Neo", "Trinity", "Amanda", "Hooper", "Tylenol", "Ibuprofen", 
    "Daffy", "Mickey", "Cashew", "Marco"
};
static const char* Radio[2] = { "False", "True" };

class Program {
private:
    // Nodes for keeping track of the sublists within master list
    // of bunnies, male_head is start of alive, data-given
    // bunnies, empty_head is the true head of list for reuse
    // of slots of dead bunnies.
    Bunny* male_head;
    Bunny* female_head;
    Bunny* radiocactive_head;
    Bunny* empty_head;

    uint_fast16_t total_population;
    uint_fast16_t radioactive_population;
    uint_fast16_t male_population;
    uint_fast16_t female_population;
public:
    void Age_Bunnies();
    void Delete_Bunny(Bunny*);
    void Breed_Bunnies();
    void Print_Bunnies();
    void Create_Bunny(uint_fast16_t mother = 0);
    uint_fast8_t Add_To_List(Bunny*);
    uint_fast8_t RNG(uint_fast8_t, uint_fast8_t);
};