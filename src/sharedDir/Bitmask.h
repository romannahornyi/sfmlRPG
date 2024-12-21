#pragma once
#include <stdint.h>

using Bitset = uint32_t;

class Bitmask {
public:
    Bitmask() : bits(0) {};
    Bitmask(const Bitset& _bits) : bits(_bits) {};

    Bitset GetMask() const { return bits; };
    void SetMask(const Bitset& _val) { bits = _val; };

    bool Matches(const Bitmask& _b, const Bitset& _relevant = 0) const {
        return (_relevant ? ((_b.GetMask() & _relevant) == (bits & _relevant)) : 
            (bits & _relevant));
    };

    void ClearBit(const unsigned int& _pos) {
        bits &= ~(1 << _pos);
    };
    void TurnOnBit(const unsigned int& _pos) {
        bits |= (1 << _pos);
    };

    void ToggleBit(const unsigned int& _pos) {
        bits ^= (1 << _pos);
    };
    void TurnOnBits(const Bitset& _bits) {
        bits |= _bits;
    };

    bool GetBit(const unsigned int& _pos) {
        return (bits & (1 << _pos) != 0);
    };
    void Clear() { bits = 0; };
private:
    Bitset bits;
};