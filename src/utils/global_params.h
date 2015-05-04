#ifndef GLOBAL_PARAMS_HEADER
#define GLOBAL_PARAMS_HEADER

#define DTYPE unsigned char
#define LTYPE float
#define FDIM 128

class GlobalParam
{
    public:
        static int Verbose;
};

// some debug functions
void showi(int i);
void showd(double d);
void showf(double f);
void shows(std::string s);

#endif  //HEADER_GLOBAL_PARAMS
