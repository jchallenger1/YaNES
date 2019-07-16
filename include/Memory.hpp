#ifndef MEMORY_HPP
#define MEMORY_HPP

#include <array>

class Memory {
    // Max amount of memory in bytes (64KB)
    static constexpr uint16_t MAXBYTES = 0xFFFF;
public:
    Memory();

    // These two read/write functions are necessary for later
    uint8_t read(const uint16_t& adr) const;
    void write(const uint16_t& adr, const uint8_t& val);

    // For 'hard writing' into memory
    uint8_t& operator[](const size_t&);
    const uint8_t& operator[](const size_t&) const;

    void clear();

    void fromFile(const std::string& fname);
private:
    std::array<uint8_t, MAXBYTES> memory{};
};

#endif // MEMORY_HPP
