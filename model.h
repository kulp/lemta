/* Inferred from libattiny*.so */
class Model_core
{
public:
    void *readVerilogSig(char const*, unsigned long*);
    void *getIntProperty(int, unsigned long*, char const*);
    void *core_getIntProperty(int, unsigned long*);
};

class Model_device
{
public:
    Model_core *getCore(unsigned int index);
    void cycle(unsigned int count);
};

class Avr8 : public Model_core
{

};

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

