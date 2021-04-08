#include <stdint.h>

class Bunny {
public:    
    Bunny* next;
private:
    // LSB is sex, M/F
    // LSB+1 is radioactive or not
    // LSB+2 is color
    // LSB remaining is age counter
    uint_fast16_t data;
    // index into a name, acts as an id
    uint_fast16_t name_index;
public:
    Bunny();
    Bunny(uint_fast16_t bunny_data, uint_fast16_t name)
        : data(bunny_data), name_index(name), next(nullptr) { }

    uint_fast16_t GetData() const { return data; }
    uint_fast16_t GetName() const { return name_index; }
    void SetData(uint_fast16_t inputData) { data = inputData; }
    void SetName(uint_fast16_t inName) { name_index = inName; }
    void Increment_Age() { data += (1 << 4); }
};