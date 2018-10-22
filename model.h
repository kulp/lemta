/* Inferred from libattiny*.so */
class Model_core;

class Model_device
{
public:
    Model_core *getCore(unsigned int index);
};

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

