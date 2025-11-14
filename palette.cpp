#include "palette.h"

// forma de invertir los bytes
// los >> o << son desplazamientos de bits derecha o izquierda
uint32_t _bswap32(uint32_t a)
{
    return ((a & 0x000000FF) << 24) |
           ((a & 0x0000FF00) << 8) |
           ((a & 0x00FF0000) >> 8) |
           ((a & 0xFF000000) >> 24);
}
std::vector<uint32_t> color_ramp = {
    /* _bswap32(0xFF1010FF),
     _bswap32(0xEF1019FF),
     _bswap32(0xE01123FF),
     _bswap32(0xD1112DFF),
     _bswap32(0xC11237FF),
     _bswap32(0xB21341FF),
     _bswap32(0xA3134BFF),
     _bswap32(0x931455FF),
     _bswap32(0x84145EFF),
     _bswap32(0x751568FF),
     _bswap32(0x651672FF),
     _bswap32(0x56167CFF),
     _bswap32(0x471786FF),
     _bswap32(0x371790FF),
     _bswap32(0x28189AFF),
     _bswap32(0x1919A4FF)*/
    _bswap32(0xF3FF0FFF),
    _bswap32(0xEDEF18FF),
    _bswap32(0xE8E022FF),
    _bswap32(0xE3D02BFF),
    _bswap32(0xDDC135FF),
    _bswap32(0xD8B23FFF),
    _bswap32(0xD3A248FF),
    _bswap32(0xCD9352FF),
    _bswap32(0xC8835BFF),
    _bswap32(0xC37465FF),
    _bswap32(0xBD656FFF),
    _bswap32(0xB85578FF),
    _bswap32(0xB34682FF),
    _bswap32(0xAD368BFF),
    _bswap32(0xA82795FF),
    _bswap32(0xA3189FFF)};