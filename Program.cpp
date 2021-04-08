#include "Program.h"

void Program::Age_Bunnies()
{
    // Start at head of master list
    Bunny* iterator = male_head;

    while (iterator != nullptr) {
        // check if this rabbit's age would kill it
        //  first check for normal bunny
        if (iterator->GetData() & (1 << 1) && 
            (iterator->GetData() >> 4) >= 10) {
                Delete_Bunny(iterator);
        } else { // its radioactive
            if ((iterator->GetData() >> 4) >= 50) {
                Delete_Bunny(iterator);
            }
        }
    }
}

void Program::Breed_Bunnies()
{
    // Can't breed if no male rabbits
    if (male_population == 0 || // if no male rabbits
        female_population == 0) { // if no female rabbits
        return;
    }

    // Iteration bunny to traverse
    Bunny* male_iter = male_head;   

    // Continue breeding until we run out of male rabbits
    while (male_iter != nullptr && male_iter != female_head) {
        if ((male_iter->GetData() >> 4) >= 2) {
            Bunny* female_iter = female_head;
            // While the female iterator is valid and not equal to the radioactive head,
            // or in short, while we have female bunnies
            while (female_iter != nullptr || female_iter != radiocactive_head) {
                if ((female_iter->GetData() >> 4) >= 2) {
                    Create_Bunny(female_iter->GetData());
                }
                female_iter = female_iter->next;
            }
        }
        male_iter = male_iter->next;
    }
}

void Program::Print_Bunnies()
{
    if (total_population == 0) {
        printf("Print: No bunnies to print.\n");
        return;
    }

    FILE * fp = fopen("output.txt", "w");
    if (fp == NULL) {
        perror("Failure: Cannot open output file 'output.txt'\n");
        return;
    }

    Bunny* iterator = male_head;
    fprintf(fp, "============== Start of Iteration Bunny List ==============\n");
    while (iterator) {        
        fprintf(fp, 
                "Bunny %s, Age: %d, Sex: %s, Color: %s, Radioactive: %s\n",
                Names[iterator->GetName()], 
                (int)(iterator->GetData() >> 4), 
                Sex[(iterator->GetData() & 1)],
                Color[(iterator->GetData() >> 2) & 0b11],
                Radio[(iterator->GetData() >> 1) & 1]
        );
        // Traverse to next rabbit
        iterator = iterator->next;
    }
    fprintf(fp, "============== End of Iteration Bunny List ==============\n");
    fclose(fp);
}

void Program::Create_Bunny(uint_fast16_t mother)
{
    // New bunnies should be age 0 and same color as their mother
    // If the mother data is not provided, then this is a call 
    // from the constructor of program

    uint_fast16_t data = 0;
    // Assign sex at random
    data |= RNG(0, 1);
    // Assign radioactive at random
    uint_fast8_t num = RNG(1, 100);
    if (num <= 2) {
        // Bunny has been to Chernobyl
        num |= (1 << 1);
    }
    // Assign Color at random or from mother if provided
    uint_fast16_t col = 0;
    if (mother == 0) {
        col = RNG(0,3);
        num |= col << 2;
    } else {
        uint_fast16_t mask = 0b1100;
        col = mother & mask;
        data |= col;
    }
    // Age is auto set to zero
    // Assign name from array of names
    uint_fast16_t name_index = RNG(0, 10);

    // Assign the data to the new bunny
    Bunny* newborn = new Bunny(data, name_index);
    // Add the newborn bunny to appropriate list

    uint_fast8_t ret = Add_To_List(newborn);
    if (!ret) {
        FILE *fp = fopen("output.txt", "w");
        if (fp != NULL) {
            fprintf(fp, "Failure: Adding newborn failed\n\tMother Data: %d, Newborn Data: %d.\n",
                (int)mother, (int)data);
        } else {
            printf("Failure: Adding newborn failed\n\tMother Data: %d, Newborn Data: %d.\n",
                (int)mother, (int)data);
        }
    }
    return;
}

uint_fast8_t Program::RNG(uint_fast8_t low, uint_fast8_t high)
{
    uint_fast8_t ran = (rand() % (high-low+1)) + low;
    return ran;
}

void Program::Delete_Bunny(Bunny* bunny)
{

}

uint_fast8_t Program::Add_To_List(Bunny* bunny)
{
    return 0;
}