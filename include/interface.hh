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

    BP_BREAKPOINT = (1 << 0),

    // Breakpoints of the following two types are commingled in one list (watches ?)
    BP_TYPE_1 = (1 << 1),
    BP_TYPE_2 = (1 << 2),

    BP_TRACEPOINT = (1 << 3),
};

// segment 0 seems to be consistently the program memory
// segment 1 seems to be consistently the data address space
// segment 2 seems to be the EEPROM
// segment 3 seems to be the bank of 32 registers, but not for all models
// segments 5 and 6 are called out specially in some compiled models
// segment 5 seems to be fuses
enum Segment {
    SEG_PROG = 0,
    SEG_DATA = 1,
    SEG_EEPROM = 2,
    SEG_REGS = 3,

    SEG_FUSES = 5,
    SEG_TYPE_6 = 6,
};

class Core;

struct Breakpoint
{
    typedef int BreakCb(Core*, Breakpoint*); // returns 0, 1, or 2 (meaning ?)

    int id; // set by addBreakpoint()
    int unknown0;
    unsigned long addr;
    char unknown1[8];
    unsigned long size;
    Segment segment;
    BPtype type;
    char unknown2[28];
    char debugName[256]; // name length here is inferred but not proven
    char unknown3[20];
    // somewhere there is a breakpoint callback among the above fields ?
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
    /*
     * pokeReg
     *
     * [0,31] -> 8-bit register
     * 0x100 -> calls dpiSetPC, returns 4
     * 0x101 -> emits error on stderr, returns -1
     * 0x102 -> calls dpiSetSP, returns 2
     * 0x103 -> calls dpiSetSreg, returns 1
     * 0x104 -> calls dpiSetCycleCounter, returns 8
     * 0x105 -> calls dpiSetLifetimeCounter, returns 8
     *
     * returns size in bytes of data used from `src`
     */
    virtual int pokeReg(int index, unsigned long src) = 0;
    virtual void* getMemoryMap() = 0;
    virtual int readMemory(unsigned long addr, unsigned long count, unsigned char *data, Segment) = 0;
    virtual int writeMemory(unsigned long addr, unsigned long count, unsigned char const *data, Segment) = 0;
    virtual int getIntProperty(int, unsigned long*, char const*) = 0;
    virtual int setIntProperty(int, unsigned long, char const*) = 0;
    virtual int getStringProperty(int index, unsigned long buflen, char *buf, unsigned long const*) = 0;
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
    virtual int getStringProperty(int index, unsigned long buflen, char *buf, unsigned long const*) = 0;
    virtual int setStringProperty(int, char*, unsigned long const*) = 0;
    virtual int saveSim(SimState*) = 0;
    virtual int loadSim(SimState*) = 0;
    virtual int debug(char const*, DebugFunc*) = 0;
    virtual int test(int, Test*, int (*)(Model*)) = 0;
};

typedef int model_api_ver_func();
typedef Model *model_ctor_func(const char *);
typedef void model_dtor_func(Model *);

extern "C" model_api_ver_func model_api_ver;
extern "C" model_ctor_func    model_ctor;
extern "C" model_dtor_func    model_dtor;

#endif

