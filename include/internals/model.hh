#ifndef MODEL_HH_
#define MODEL_HH_

#include "interface.hh"

/* Inferred from libattiny*.so */

#include <deque>
#include <map>
#include <string>
#include <vector>

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

// ABI compatibility cannot be maintained, so SimProperty does not have an
// actual definition, but it consists of an int value followed by a
// std::string. sizeof(SimProperty) is 24 bytes.
struct SimProperty;

typedef void *UnknownType;

class Model_core : public Core
{
public:
    Model_core(Model_device *, int);
    ~Model_core();

protected:
    UnknownType readVerilogSig(char const *, unsigned long *);
    UnknownType deleteBreakpointFrom(int,
                                     std::multimap<unsigned int, Breakpoint> &);
    UnknownType insertBreakpointTo(Breakpoint &,
                                   std::multimap<unsigned int, Breakpoint> &);
    UnknownType execStepCallback();
    UnknownType checkBreak(std::deque<Breakpoint *> &, Breakpoint *&,
                           Instruction, Address);
    UnknownType checkTrace(std::deque<Breakpoint *> &, Breakpoint *&);
    UnknownType checkWatch(std::deque<Breakpoint *> &, Breakpoint *&);

    // order of field m_nextBreakId not known
    UnknownType m_nextBreakId;

    // byte offset of breakpoints is 0xe8 on Linux
    // Breakpoint **breakpoints;

    // byte offset of top is 0x158 on Linux
    // Vsim_top *top;

    // byte offset of running is 0x170 on Linux
    // bool running;

    // byte offset of facade is 0x1b0 on Linux
    // MemFacade facade;

    // byte offset of Model * is 0x1c0 on Linux
    // Model *model;

    // byte offset of props is 0x1d8 on Linux
    // std::map<int const, SimProperty> props;

public:
    // inherited from Core
    virtual int reset(ResetType);
    virtual int step(Address);
    virtual Address run(Address pc);
    virtual void stop();
    virtual int addStepCallback(void (*)(Core *, void *), void *);
    virtual int removeStepCallback(int);
    virtual int peekReg(int, unsigned long *);
    virtual int pokeReg(int, unsigned long);
    // getMemoryMap() is implemented trivially : "getMemoryMap should be
    // implemented in children core class"
    virtual void *getMemoryMap();
    virtual int readMemory(Address, unsigned long, unsigned char *,
                           Segment);
    virtual int writeMemory(Address, unsigned long, unsigned char const *,
                            Segment);
    virtual int getIntProperty(int, unsigned long *, char const *);
    virtual int setIntProperty(int, unsigned long, char const *);
    virtual int getStringProperty(int, unsigned long, char *,
                                  unsigned long const *);
    // setStringProperty() is implemented trivially : `return -1`
    virtual int setStringProperty(int, char *, unsigned long const *);
    virtual Model *getModel();
    virtual int addBreakpoint(Breakpoint *);
    virtual int removeBreakpoint(int);
    virtual Breakpoint **getBreakpoints(BPtype);
    // test() is implemented trivially : `return -1`
    virtual int test(int, Test *, int (*)(Model *));

protected:
    // in Model_core only
    virtual void core_init();
    virtual void core_config(char const *);
    virtual int core_reset(ResetType);
    virtual bool core_isBreakInstr(unsigned int);
    virtual int core_getIntProperty(int, unsigned long *);
    virtual int core_setIntProperty(int, unsigned long);
};

class Avr8 : public Model_core
{
public:
    Avr8(Model_device *, int);
    ~Avr8();

    // inherited from Core
    virtual int writeMemory(Address, unsigned long, unsigned char const *,
                            Segment);
    virtual int peekReg(int, unsigned long *);
    virtual int pokeReg(int, unsigned long);
    virtual int readMemory(Address, unsigned long, unsigned char *,
                           Segment);

protected:
    virtual void core_init();
    // core_config is implemented trivially : `return`
    virtual void core_config(char const *);
    // core_reset() is implemented trivially : `return 0`
    virtual int core_reset(ResetType);
    virtual bool core_isBreakInstr(unsigned int);
    virtual int core_getIntProperty(int, unsigned long *);
    // core_setIntProperty() is implemented trivially : `return -1`
    virtual int core_setIntProperty(int, unsigned long);
};

class Model_device : public Model
{
public:
    Model_device(char const *);
    ~Model_device();
    UnknownType dev_config(char const *);
    UnknownType dev_init(char const *);
    UnknownType dev_reset(ResetType);
    UnknownType getHaltReason();
    UnknownType step(int);

    UnknownType removePendingBreak(Breakpoint *);

    // order of field not known
    UnknownType m_timeStamp;

    // byte offset of Core * array is 0x28 on Linux
    // Core **cores;

    // byte offset of props is 0x170 on Linux
    // std::map<int const, SimProperty> props;

    // inherited from Model
    virtual Core *getCore(unsigned int);
    virtual int reset(ResetType);
    virtual void cycle(unsigned int);
    virtual void stop();
    virtual int addCycleCallback(void (*)(Model *, void *), void *);
    virtual int removeCycleCallback(int);
    virtual int getIntProperty(int, unsigned long *, char const *);
    // setIntProperty() is implemented trivially : `return -1`
    virtual int setIntProperty(int, unsigned long, char const *);
    // getStringProperty() is implemented trivially : `return -1`
    virtual int getStringProperty(int, unsigned long, char *,
                                  unsigned long const *);
    // setStringProperty() is implemented trivially : `return -1`
    virtual int setStringProperty(int, char *, unsigned long const *);
    // saveSim() is implemented trivially : `return -1`
    virtual int saveSim(SimState *);
    // loadSim() is implemented trivially : `return -1`
    virtual int loadSim(SimState *);
    // debug() is implemented trivially : `return 0`
    virtual int debug(char const *, int (*)(char const *, ...));
    // test() is implemented trivially : `return -1`
    virtual int test(int, Test *, int (*)(Model *));
};

class model_verilated
{
public:
    virtual ~model_verilated();

    virtual UnknownType eval() = 0;
    virtual UnknownType dpiGetLifetimeCounter(unsigned int *) = 0;
    virtual UnknownType dpiSetLifetimeCounter(unsigned int const *) = 0;
    virtual UnknownType dpiGetSignature(unsigned int const *) = 0;
    virtual UnknownType dpiGetSib(unsigned int *) = 0;
    virtual UnknownType dpiSetDevSel(unsigned int const *,
                                     unsigned int const *) = 0;
    virtual UnknownType dpiTick(unsigned int *) = 0;
    virtual UnknownType dpiGetTimeStep() = 0;
    virtual UnknownType dpiGetMainClk() = 0;
    virtual UnknownType dpiGetPC(int) = 0;
    virtual UnknownType dpiGetInstr(int) = 0;
    virtual UnknownType dpiGetSP(int) = 0;
    virtual UnknownType dpiGetSreg(int) = 0;
    virtual UnknownType dpiPeekReg(unsigned int const *, int) = 0;
    virtual UnknownType dpiPokeReg(unsigned int const *, unsigned int const *,
                                   int) = 0;
    virtual UnknownType dpiSetPC(unsigned int const *, int) = 0;
    virtual UnknownType dpiSetSP(unsigned int const *, int) = 0;
    virtual UnknownType dpiSetSreg(unsigned int const *, int) = 0;
    virtual UnknownType dpiGetCycleCounter(unsigned int *, int) = 0;
    virtual UnknownType dpiSetCycleCounter(unsigned int const *, int) = 0;
    virtual UnknownType dpiStartReset(unsigned int const *, int) = 0;
    virtual UnknownType dpiStopReset(int) = 0;
    virtual UnknownType dpiPeekMemory(unsigned int const *addr, unsigned int *,
                                      unsigned int const *segment,
                                      unsigned int *data, int) = 0;
    virtual UnknownType dpiPokeMemory(unsigned int const *addr, unsigned int *,
                                      unsigned int const *segment,
                                      unsigned int const *data, int) = 0;
    virtual UnknownType dpiPeekFuses(unsigned int const *) = 0;
    virtual UnknownType dpiPokeFuses(unsigned int const *,
                                     unsigned int const *) = 0;
    virtual UnknownType dpiGetSupportedAccessType(unsigned int const *,
                                                  int) = 0;
    virtual Address     dpiCheckWatchHit(unsigned int const *,
                                         unsigned int const *, unsigned int *,
                                         unsigned int const *, int) = 0;
    virtual UnknownType dpiGetModelState(int) = 0;
    virtual UnknownType dpiInstrFinished(int) = 0;
};

// Vsim_top multiply inherits from model_verilated and from VerilatedModule.
// Apparently model_verilated is a pure-virtual interface used called
// internally but not exposed through the DLL API/ABI.

class MemUnit
{
public:
    virtual ~MemUnit();

    virtual std::string const &getName() const = 0;
    virtual UnknownType isChanged(Address, unsigned long) = 0;
    virtual UnknownType update(Address, unsigned long) = 0;
    virtual UnknownType read(Address, unsigned long,
                             std::vector<unsigned char> &) = 0;
    virtual UnknownType write(Address, unsigned long,
                              std::vector<unsigned char> const &) = 0;
    virtual UnknownType getAddress() const = 0;
    virtual UnknownType getSize() const = 0;
};

class BasicMemUnit : public MemUnit
{
public:
    virtual ~BasicMemUnit();

    // inherited from MemUnit
    virtual std::string const &getName() const;
    virtual UnknownType isChanged(Address, unsigned long);
    virtual UnknownType update(Address, unsigned long);
    virtual UnknownType read(Address, unsigned long,
                             std::vector<unsigned char> &);
    virtual UnknownType write(Address, unsigned long,
                              std::vector<unsigned char> const &);
    virtual UnknownType getAddress() const;
    virtual UnknownType getSize() const;
};

class VerilogMemUnit : public MemUnit
{
public:
    virtual ~VerilogMemUnit();

    // inherited from BasicMemUnit
    virtual std::string const &getName() const;
    virtual UnknownType isChanged(Address, unsigned long);
    virtual UnknownType update(Address, unsigned long);
    virtual UnknownType read(Address, unsigned long,
                             std::vector<unsigned char> &);
    virtual UnknownType write(Address, unsigned long,
                              std::vector<unsigned char> const &);
    virtual UnknownType getAddress() const;
    virtual UnknownType getSize() const;

    // VerilogMemUnit only
    virtual UnknownType getBits() const;
};

class MemFacade
{
public:
    MemFacade(model_verilated *, bool, int);
    virtual ~MemFacade();

    // write() implements Model_core::writeMemory
    int write(unsigned int addr, unsigned int count, unsigned char const *data,
              unsigned int segment);
    UnknownType exists(unsigned int) const;
    MemUnit *getMemUnit(unsigned int);
    MemUnit *getMemUnit(std::string const &);
    // read() implements Model_core::readMemory
    int read(unsigned int addr, unsigned int count, unsigned char *data,
             unsigned int segment);
    UnknownType addMemUnit(unsigned int, MemUnit *);
    UnknownType addMemUnits(std::map<unsigned int, MemUnit *> const &);
};

#endif
