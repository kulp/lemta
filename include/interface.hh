#ifndef INTERFACE_HH_
#define INTERFACE_HH_

// TODO enumerate reset types and determine meanings
enum ResetType
{
    RT_TYPE_0 = 0,
};

enum BPtype
{
    BP_BREAKPOINT = (1 << 0),

    BP_WATCH_A = (1 << 1),
    BP_WATCH_B = (1 << 2),

    BP_TRACEPOINT = (1 << 3),

    BP_ALL_TYPES = (1 << 4) - 1
};

enum Segment
{
    SEG_PROG = 0,
    SEG_DATA = 1,
    SEG_EEPROM = 2,
    SEG_REGS = 3,

    SEG_FUSES = 5,
    SEG_TYPE_6 = 6,
};

class Core;

struct Breakpoint // a POD type
{
    typedef int BreakCb(Core *, Breakpoint *); // returns 0, 1, or 2 (meaning ?)

    int id; // set by addBreakpoint()
    // 4 bytes padding (?)
    unsigned long addr;
    long unknown1;
    unsigned long size;
    Segment segment;
    BPtype type;
    long unknown2[3];
    unsigned int hitcount;
    char debugName[256];
    // 4 bytes padding (?)
    BreakCb *handler;
    void *userdata;
};

class Model;
struct Test; // a POD type, contents yet unknown

class Core
{
public:
    typedef void StepCb(Core *, void *);

    virtual int reset(ResetType) = 0;
    virtual int step(unsigned long) = 0;
    // run() runs to a given program counter value
    virtual unsigned long run(unsigned long pc) = 0;
    virtual void stop() = 0; // could be called by a StepCb
    virtual int addStepCallback(StepCb *, void *) = 0;
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
    virtual void *getMemoryMap() = 0;
    virtual int readMemory(unsigned long addr, unsigned long count,
                           unsigned char *data, Segment) = 0;
    virtual int writeMemory(unsigned long addr, unsigned long count,
                            unsigned char const *data, Segment) = 0;
    virtual int getIntProperty(int, unsigned long *, char const *) = 0;
    virtual int setIntProperty(int, unsigned long, char const *) = 0;
    virtual int getStringProperty(int index, unsigned long buflen, char *buf,
                                  unsigned long const *) = 0;
    virtual int setStringProperty(int, char *, unsigned long const *) = 0;
    virtual Model *getModel() = 0;
    virtual int addBreakpoint(Breakpoint *) = 0;
    virtual int removeBreakpoint(int) = 0;
    // getBreakpoints() returns a nullptr-terminated array member of Core
    virtual Breakpoint **getBreakpoints(BPtype) = 0;
    virtual int test(int, Test *, int (*)(Model *)) = 0;
};

typedef void SimState;

class Model
{
public:
    typedef void CycleCb(Model *, void *);
    typedef int DebugFunc(char const *, ...);

    virtual Core *getCore(unsigned int) = 0;
    virtual int reset(ResetType) = 0;
    virtual void cycle(unsigned int) = 0;
    virtual void stop() = 0;
    virtual int addCycleCallback(CycleCb *, void *) = 0;
    virtual int removeCycleCallback(int) = 0;
    virtual int getIntProperty(int, unsigned long *, char const *) = 0;
    virtual int setIntProperty(int, unsigned long, char const *) = 0;
    virtual int getStringProperty(int index, unsigned long buflen, char *buf,
                                  unsigned long const *) = 0;
    virtual int setStringProperty(int, char *, unsigned long const *) = 0;
    virtual int saveSim(SimState *) = 0;
    virtual int loadSim(SimState *) = 0;
    virtual int debug(char const *, DebugFunc *) = 0;
    virtual int test(int, Test *, int (*)(Model *)) = 0;
};

typedef int model_api_ver_func();
typedef Model *model_ctor_func(const char *);
typedef void model_dtor_func(Model *);

extern "C" model_api_ver_func model_api_ver;
extern "C" model_ctor_func model_ctor;
extern "C" model_dtor_func model_dtor;

#endif
