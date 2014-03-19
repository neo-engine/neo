static const long hextable[] = {
    [0 ... 255] = -1, // bit aligned access into this table is considerably
    ['0'] = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, // faster for most modern processors,
    ['A'] = 10, 11, 12, 13, 14, 15,       // for the space conscious, reduce to
    ['a'] = 10, 11, 12, 13, 14, 15        // signed char.
};
long hexdec(unsigned const char *hex) {
    long ret = 0; 
    while (*hex && ret >= 0) 
        ret = (ret << 4) | hextable[*hex++];
    return ret; 
}