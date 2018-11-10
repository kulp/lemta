#ifndef INTERFACE_HH_
#define INTERFACE_HH_

class Model;

// inferred from mangled signatures only
class Breakpoint;
class SimState;
class Test;

// placeholder types
typedef void *UnknownType;
// forward declare C++11 enumerations for which actual values are not yet known
enum class ResetType : int;
enum class BPtype : int;

// inferred from Model_core::writeMemory
typedef unsigned int Segment;

class Core
{
public:
    typedef void StepCb(Core*, void*);

    virtual UnknownType reset(ResetType) = 0;
    virtual UnknownType step(unsigned long) = 0;
    virtual UnknownType run(unsigned long) = 0;
    virtual UnknownType stop() = 0;
    virtual UnknownType addStepCallback(StepCb*, void*) = 0;
    virtual UnknownType removeStepCallback(int) = 0;
    virtual UnknownType peekReg(int, unsigned long*) = 0;
    virtual UnknownType pokeReg(int, unsigned long) = 0;
    virtual UnknownType getMemoryMap() = 0;
    virtual UnknownType readMemory(unsigned long, unsigned long, unsigned char*, Segment) = 0;
    virtual UnknownType writeMemory(unsigned long, unsigned long, unsigned char const*, Segment) = 0;
    virtual int getIntProperty(int, unsigned long*, char const*) = 0;
    virtual UnknownType setIntProperty(int, unsigned long, char const*) = 0;
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*) = 0;
    virtual UnknownType setStringProperty(int, char*, unsigned long const*) = 0;
    virtual Model *getModel() = 0;
    virtual UnknownType addBreakpoint(Breakpoint*) = 0;
    virtual UnknownType removeBreakpoint(int) = 0;
    virtual UnknownType getBreakpoints(BPtype) = 0;
    virtual int test(int, Test*, int (*)(Model*)) = 0;
};

class Model
{
public:
    typedef void CycleCb(Model*, void*);
    typedef int DebugFunc(char const*, ...);

    virtual Core *getCore(unsigned int) = 0;
    virtual UnknownType reset(ResetType) = 0;
    virtual UnknownType cycle(unsigned int) = 0;
    virtual UnknownType stop() = 0;
    virtual UnknownType addCycleCallback(CycleCb*, void*) = 0;
    virtual UnknownType removeCycleCallback(int) = 0;
    virtual int getIntProperty(int, unsigned long*, char const*) = 0;
    virtual UnknownType setIntProperty(int, unsigned long, char const*) = 0;
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*) = 0;
    virtual UnknownType setStringProperty(int, char*, unsigned long const*) = 0;
    virtual UnknownType saveSim(SimState*) = 0;
    virtual UnknownType loadSim(SimState*) = 0;
    virtual int debug(char const*, DebugFunc*) = 0;
    virtual int test(int, Test*, int (*)(Model*)) = 0;
};

class model_verilated
{
public:
    virtual ~model_verilated();

    virtual UnknownType eval() = 0;
    virtual UnknownType dpiGetLifetimeCounter(unsigned int*) = 0;
    virtual UnknownType dpiSetLifetimeCounter(unsigned int const*) = 0;
    virtual UnknownType dpiGetSignature(unsigned int const*) = 0;
    virtual UnknownType dpiGetSib(unsigned int*) = 0;
    virtual UnknownType dpiSetDevSel(unsigned int const*, unsigned int const*) = 0;
    virtual UnknownType dpiTick(unsigned int*) = 0;
    virtual UnknownType dpiGetTimeStep() = 0;
    virtual UnknownType dpiGetMainClk() = 0;
    virtual UnknownType dpiGetPC(int) = 0;
    virtual UnknownType dpiGetInstr(int) = 0;
    virtual UnknownType dpiGetSP(int) = 0;
    virtual UnknownType dpiGetSreg(int) = 0;
    virtual UnknownType dpiPeekReg(unsigned int const*, int) = 0;
    virtual UnknownType dpiPokeReg(unsigned int const*, unsigned int const*, int) = 0;
    virtual UnknownType dpiSetPC(unsigned int const*, int) = 0;
    virtual UnknownType dpiSetSP(unsigned int const*, int) = 0;
    virtual UnknownType dpiSetSreg(unsigned int const*, int) = 0;
    virtual UnknownType dpiGetCycleCounter(unsigned int*, int) = 0;
    virtual UnknownType dpiSetCycleCounter(unsigned int const*, int) = 0;
    virtual UnknownType dpiStartReset(unsigned int const*, int) = 0;
    virtual UnknownType dpiStopReset(int) = 0;
    virtual UnknownType dpiPeekMemory(unsigned int const*, unsigned int*, unsigned int const*, unsigned int*, int) = 0;
    virtual UnknownType dpiPokeMemory(unsigned int const*, unsigned int*, unsigned int const*, unsigned int const*, int) = 0;
    virtual UnknownType dpiPeekFuses(unsigned int const*) = 0;
    virtual UnknownType dpiPokeFuses(unsigned int const*, unsigned int const*) = 0;
    virtual UnknownType dpiGetSupportedAccessType(unsigned int const*, int) = 0;
    virtual UnknownType dpiCheckWatchHit(unsigned int const*, unsigned int const*, unsigned int*, unsigned int const*, int) = 0;
    virtual UnknownType dpiGetModelState(int) = 0;
    virtual UnknownType dpiInstrFinished(int) = 0;
};

// Vsim_top multiply inherits from model_verilated and from VerilatedModule
// Presumably model_verilated is the pure-virtual interface exposed through the
// DLL API/ABI, and the rest of the Verilator implementation is left opaque.

typedef int model_api_ver_func();
typedef Model *model_ctor_func(const char *);
typedef int model_dtor_func(Model *);

extern "C" model_api_ver_func model_api_ver;
extern "C" model_ctor_func    model_ctor;
extern "C" model_dtor_func    model_dtor;

#endif

