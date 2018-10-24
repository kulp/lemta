/* Inferred from libattiny*.so */

// inferred from presence of typeinfo
class Avr8;
class BasicMemUnit;
class Core;
class MemFacade;
class MemUnit;
class Model;
class Model_core;
class Model_device;
class VerilatedModule;
class VerilatedVcdFile;
class VerilogMemUnit;
class Vsim_top;
class model_verilated;

// inferred from mangled signatures only
class Breakpoint;
class SimState;
class Test;

// placeholder types
typedef void *UnknownType;
enum ResetType { };
enum BPtype { };

// inferred from Model_core::writeMemory
typedef unsigned int Segment;

class Core
{
public:
    virtual UnknownType reset(ResetType) = 0;
    virtual UnknownType step(unsigned long) = 0;
    virtual UnknownType run(unsigned long) = 0;
    virtual UnknownType stop() = 0;
    virtual UnknownType addStepCallback(void (*)(Core*, void*), void*) = 0;
    virtual UnknownType removeStepCallback(int) = 0;
    virtual UnknownType peekReg(int, unsigned long*) = 0;
    virtual UnknownType pokeReg(int, unsigned long) = 0;
    virtual UnknownType getMemoryMap() = 0;
    virtual UnknownType readMemory(unsigned long, unsigned long, unsigned char*, Segment) = 0;
    virtual UnknownType writeMemory(unsigned long, unsigned long, unsigned char const*, Segment) = 0;
    virtual UnknownType getIntProperty(int, unsigned long*, char const*) = 0;
    virtual UnknownType setIntProperty(int, unsigned long, char const*) = 0;
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*) = 0;
    virtual UnknownType setStringProperty(int, char*, unsigned long const*) = 0;
    virtual Model_device *getModel() = 0;
    virtual UnknownType addBreakpoint(Breakpoint*) = 0;
    virtual UnknownType removeBreakpoint(int) = 0;
    virtual UnknownType getBreakpoints(BPtype) = 0;
    virtual UnknownType test(int, Test*, int (*)(Model*)) = 0;
};

class Model_core : public Core
{
public:
    UnknownType readVerilogSig(char const*, unsigned long*);

    // inherited from Core
    virtual UnknownType reset(ResetType);
    virtual UnknownType step(unsigned long);
    virtual UnknownType run(unsigned long);
    virtual UnknownType stop();
    virtual UnknownType addStepCallback(void (*)(Core*, void*), void*);
    virtual UnknownType removeStepCallback(int);
    virtual UnknownType peekReg(int, unsigned long*);
    virtual UnknownType pokeReg(int, unsigned long);
    virtual UnknownType getMemoryMap();
    virtual UnknownType readMemory(unsigned long, unsigned long, unsigned char*, Segment);
    virtual UnknownType writeMemory(unsigned long, unsigned long, unsigned char const*, Segment);
    virtual UnknownType getIntProperty(int, unsigned long*, char const*);
    virtual UnknownType setIntProperty(int, unsigned long, char const*);
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*);
    virtual UnknownType setStringProperty(int, char*, unsigned long const*);
    virtual Model_device *getModel();
    virtual UnknownType addBreakpoint(Breakpoint*);
    virtual UnknownType removeBreakpoint(int);
    virtual UnknownType getBreakpoints(BPtype);
    virtual UnknownType test(int, Test*, int (*)(Model*));

    // in Model_core only
    virtual UnknownType core_init();
    virtual UnknownType core_config(char const*);
    virtual UnknownType core_reset(ResetType);
    virtual UnknownType core_isBreakInstr(unsigned int);
    virtual UnknownType core_getIntProperty(int, unsigned long*);
    virtual UnknownType core_setIntProperty(int, unsigned long);
};

class Model
{
public:
    virtual Model_core *getCore(unsigned int);
    virtual UnknownType reset(ResetType);
    virtual UnknownType cycle(unsigned int);
    virtual UnknownType stop();
    virtual UnknownType addCycleCallback(void (*)(Model*, void*), void*);
    virtual UnknownType removeCycleCallback(int);
    virtual UnknownType getIntProperty(int, unsigned long*, char const*);
    virtual UnknownType setIntProperty(int, unsigned long, char const*);
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*);
    virtual UnknownType setStringProperty(int, char*, unsigned long const*);
    virtual UnknownType saveSim(SimState*);
    virtual UnknownType loadSim(SimState*);
    virtual UnknownType debug(char const*, int (*)(char const*, ...));
    virtual UnknownType test(int, Test*, int (*)(Model*));
};

class Model_device : public Model
{
public:
    // inherited from Model
    virtual Model_core *getCore(unsigned int);
    virtual UnknownType reset(ResetType);
    virtual UnknownType cycle(unsigned int);
    virtual UnknownType stop();
    virtual UnknownType addCycleCallback(void (*)(Model*, void*), void*);
    virtual UnknownType removeCycleCallback(int);
    virtual UnknownType getIntProperty(int, unsigned long*, char const*);
    virtual UnknownType setIntProperty(int, unsigned long, char const*);
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*);
    virtual UnknownType setStringProperty(int, char*, unsigned long const*);
    virtual UnknownType saveSim(SimState*);
    virtual UnknownType loadSim(SimState*);
    virtual UnknownType debug(char const*, int (*)(char const*, ...));
    virtual UnknownType test(int, Test*, int (*)(Model*));
};

class Avr8 : public Model_core
{
    // no other virtual methods
};

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

