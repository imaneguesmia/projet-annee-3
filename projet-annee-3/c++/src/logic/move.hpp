#pragma once

#include <cstdint>
#include <iostream>

// Never thought I'd actually use bitfields ngl
struct Move {
    uint8_t source      : 6;
    uint8_t target      : 6;
    uint8_t piece       : 4;
    uint8_t promotion   : 4;
    bool capture        : 1;
    bool double_push    : 1;
    bool en_passant     : 1;
    bool castle         : 1;
};

std::ostream& operator<<(std::ostream& out, const Move& move);