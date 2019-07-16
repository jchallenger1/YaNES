#include "Memory.hpp"

#include <fstream>
#include <iostream>


inline bool inRange(const uint8_t& min, const uint8_t& max, const uint8_t& val) {
    return val <= max && val >= min;
}

Memory::Memory() {
    ;
}

// These two functions will be much more in depth due to mirrors later.
uint8_t Memory::read(const uint16_t& adr) const {
    return memory[adr];
}

void Memory::write(const uint16_t& adr, const uint8_t& val) {
    memory[adr] = val;
}

void Memory::clear() {
    memory.fill(0);
}


uint8_t& Memory::operator[](const size_t& index) {
    return memory[index];
}

const uint8_t& Memory::operator[](const size_t& index) const {
    return memory[index];
}


void Memory::fromFile(const std::string& fname) {
    std::ifstream ifs(fname, std::ios_base::binary | std::ios_base::in | std::ios_base::ate);
    if (!ifs.good()) {
        std::cerr << "File not found" << std::endl;
        throw std::runtime_error("File not found, given path:" + fname);
    }
    uint8_t byte;
    auto read = [&ifs, &byte]() -> uint8_t {
            ifs.read(reinterpret_cast<char*>(&byte), sizeof (uint8_t) );
            return byte;
    };
    bool isNESFile = read() == 'N' && read() == 'E' && read() == 'S';
    if (!isNESFile) {
        std::cerr << "Given file is not an NES 2.0 file";
        throw std::runtime_error("Unsupported file type");
    }


}
