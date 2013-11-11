int is_within_int(int cmp1, int cmp2, int fudge) {
    if(cmp1 >= cmp2-fudge && cmp1 <= cmp2+fudge) {
        return 1;
    }
    return 0;
}
float is_within_float(float cmp1, float cmp2, float fudge) {
    if(cmp1 >= cmp2-fudge && cmp1 <= cmp2+fudge) {
        return 1;
    }
    return 0;
}
