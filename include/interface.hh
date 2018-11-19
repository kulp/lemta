#ifndef INTERFACE_HH_
#define INTERFACE_HH_

class Model;

struct Breakpoint; // no methods
typedef void SimState; // apparently unused
struct Test; // no methods

// placeholder types
// forward declare C++11 enumerations for which actual values are not yet known
enum class ResetType : int;
enum BPtype {
    BP_NO_TYPE = 0,

    // Breakpoints of this type exist in their own list (breaks ?)
    BP_TYPE_0 = (1 << 0),

    // Breakpoints of the following two types are commingled in one list (watches ?)
    BP_TYPE_1 = (1 << 1),
    BP_TYPE_2 = (1 << 2),

    BP_TRACEPOINT = (1 << 3),
};
enum class Segment : int;

struct Breakpoint
{
    char padding0[8];
    unsigned long addr; // @ 0x8
    char padding1[8];
    unsigned long size; // @ 0x18
    Segment segment; // @ 0x20
    BPtype type; // @ 0x24
    char padding2[28];
    bool b000; // @ 0x44

    // somewhere there is a breakpoint callback too ?
    char buf[128]; // unknown extent
};

class Core
{
public:
    typedef void StepCb(Core*, void*);

    virtual int reset(ResetType) = 0;
    virtual int step(unsigned long) = 0;
    virtual unsigned long run(unsigned long pc) = 0; // run to a given program counter value
    virtual void stop() = 0; // could be called by a StepCb
    virtual int addStepCallback(StepCb*, void*) = 0;
    virtual int removeStepCallback(int) = 0;
    /*
     * peekReg
     *
     * [0,31] -> 8-bit register
     * 0x100 -> calls dpiGetPC, returns 4
     * 0x101 -> calls dpiGetInstr, returns 4
     * 0x102 -> calls dpiGetSP, returns 2
     * 0x103 -> calls dpiGetSreg, returns 1
     * 0x104 -> calls dpiGetCycleCounter, returns 8
     * 0x105 -> calls dpiGetLifetimeCounter, returns 8
     *
     * returns size in bytes of data returned in `dst`
     */
    virtual int peekReg(int index, unsigned long *dst) = 0;
    virtual int pokeReg(int index, unsigned long src) = 0;
    virtual void* getMemoryMap() = 0;
    virtual int readMemory(unsigned long, unsigned long, unsigned char*, Segment) = 0;
    virtual int writeMemory(unsigned long, unsigned long, unsigned char const*, Segment) = 0;
    virtual int getIntProperty(int, unsigned long*, char const*) = 0;
    virtual int setIntProperty(int, unsigned long, char const*) = 0;
    virtual int getStringProperty(int, unsigned long, char*, unsigned long const*) = 0;
    virtual int setStringProperty(int, char*, unsigned long const*) = 0;
    virtual Model *getModel() = 0;
    virtual int addBreakpoint(Breakpoint*) = 0;
    virtual int removeBreakpoint(int) = 0;
    virtual Breakpoint** getBreakpoints(BPtype) = 0; // returns a nullptr-terminated array member of Core
    virtual int test(int, Test*, int (*)(Model*)) = 0;
};

class Model
{
public:
    typedef void CycleCb(Model*, void*);
    typedef int DebugFunc(char const*, ...);

    virtual Core *getCore(unsigned int) = 0;
    virtual int reset(ResetType) = 0;
    virtual void cycle(unsigned int) = 0;
    virtual void stop() = 0;
    virtual int addCycleCallback(CycleCb*, void*) = 0;
    virtual int removeCycleCallback(int) = 0;
    virtual int getIntProperty(int, unsigned long*, char const*) = 0;
    virtual int setIntProperty(int, unsigned long, char const*) = 0;
    virtual int getStringProperty(int, unsigned long, char*, unsigned long const*) = 0;
    virtual int setStringProperty(int, char*, unsigned long const*) = 0;
    virtual int saveSim(SimState*) = 0;
    virtual int loadSim(SimState*) = 0;
    virtual int debug(char const*, DebugFunc*) = 0;
    virtual int test(int, Test*, int (*)(Model*)) = 0;
};

typedef int model_api_ver_func();
typedef Model *model_ctor_func(const char *);
typedef int model_dtor_func(Model *);

extern "C" model_api_ver_func model_api_ver;
extern "C" model_ctor_func    model_ctor;
extern "C" model_dtor_func    model_dtor;

#endif

