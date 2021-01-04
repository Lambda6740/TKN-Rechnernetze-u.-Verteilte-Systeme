void strcpy_len(char *des, char *src, int len)
{
    for(int i = 0; i < len; i++){
        *des=*src;
        des++;
        src++;
    }
}

