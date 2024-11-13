#include "bitboard.hpp"

/* ---- DEFINE namespace BB ---- */

void BB::out(std::ostream& stream, const BitBoard& bits) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int position {i*8 + j};

            stream << (get_bit(bits, position) ? '1' : '0')  << ' ';
        }

        stream << std::endl;
    }
}

/* ---- END DEFINE ---- */