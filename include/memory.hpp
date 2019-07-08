#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>

class Memory {
    // Max amount of memory in bytes (64KB)
    static constexpr uint16_t MAXBYTES = 0xFFFF;
public:
    Memory();

    uint8_t read(const uint8_t& adr) const;
    void write(const uint8_t& adr, const uint8_t& val);

private:
    std::array<uint8_t, MAXBYTES> memory{};
};

#endif // MEMORY_HPP
