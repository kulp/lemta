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
    BP_WATCH_WRITE = (1 << 1),
    BP_WATCH_READ = (1 << 2),
    BP_TRACEPOINT = (1 << 3),
    BP_FOUND = (1 << 4), // i.e. a break instruction in flash

    BP_ALL_TYPES = -1u
};

enum Segment
{
    SEG_PROG = 0,
    SEG_DATA = 1,
    SEG_IO = 2,
    SEG_REGS = 3,

    SEG_FUSES = 5,
    SEG_TYPE_6 = 6,
};

enum RegisterSpecial
{
    REG_PC = 0x100,       // 4 bytes wide
    REG_INSTR = 0x101,    // 4 bytes wide
    REG_SP = 0x102,       // 2 bytes wide
    REG_SREG = 0x103,     // 1 byte wide
    REG_CYCLES = 0x104,   // 8 bytes wide
    REG_LIFETIME = 0x105, // 8 bytes wide
};

enum BreakResult
{
    BR_CONTINUE = 0,
    BR_TYPE_1 = 1,
    BR_TYPE_2 = 2,
};

class Core;

typedef unsigned int Instruction;
typedef unsigned long Address;

struct Breakpoint
{
    typedef BreakResult BreakCb(Core *, Breakpoint *);

    int id;       // set by addBreakpoint()
    int unknown0; // initialized to *(&core+0x1d0)
    Address addr;
    Address addr2; // addr_end, inclusive ?
    unsigned long size;
    Segment segment;
    BPtype type;
    Address watch_base_addr; // return value of dpiCheckWatchHit
    int dword0;    // output from third param of dpiCheckWatchHit
    // 4 bytes padding @ offset 0x34 = 52
    Address break_pc; // program counter of most recent breakpoint hit
    unsigned int hitcount;
    char debug_name[256];
    // 4 bytes padding @ offset 0x144 = 324
    BreakCb *handler;
    void *userdata;

    Breakpoint()
        : unknown0(-1)
        , addr(-1ul)
        , addr2(-1ul)
        , size(1)
        , segment(SEG_PROG)
        , type(BP_BREAKPOINT)
        , watch_base_addr(-1ul)
        , dword0()
        , break_pc(-1ul)
        , hitcount(0)
        , debug_name()
        , handler()
        , userdata()
    {
        // no body
    }
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
     * [0x100, 0x105] -> special register (see enum RegisterSpecial)
     * returns size in bytes of data returned in `dst`
     */
    virtual int peekReg(int index, unsigned long *dst) = 0;
    /*
     * pokeReg
     *
     * [0,31] -> 8-bit register
     * 0x101 -> emits error on stderr, returns -1
     * [0x100, 0x105] -> special register (see enum RegisterSpecial)
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
