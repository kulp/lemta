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

class Model_core : public Core
{
public:
    Model_core(Model_device*, int);
    ~Model_core();
protected:
    UnknownType readVerilogSig(char const*, unsigned long*);
    UnknownType deleteBreakpointFrom(int, std::multimap<unsigned int, Breakpoint>&);
    // need definition of Breakpoint type
    //UnknownType insertBreakpointTo(Breakpoint&, std::multimap<unsigned int, Breakpoint> >&);
    UnknownType execStepCallback();
    UnknownType checkBreak(std::deque<Breakpoint*>&, Breakpoint*&, unsigned int, unsigned long);
    UnknownType checkTrace(std::deque<Breakpoint*>&, Breakpoint*&);
    UnknownType checkWatch(std::deque<Breakpoint*>&, Breakpoint*&);

    // order of field not known
    UnknownType m_nextBreakId;

public:
    // inherited from Core
    virtual UnknownType reset(ResetType);
    virtual UnknownType step(unsigned long);
    virtual UnknownType run(unsigned long);
    virtual UnknownType stop();
    virtual UnknownType addStepCallback(void (*)(Core*, void*), void*);
    virtual UnknownType removeStepCallback(int);
    virtual UnknownType peekReg(int, unsigned long*);
    virtual UnknownType pokeReg(int, unsigned long);
    virtual UnknownType getMemoryMap(); // implemented trivially : "getMemoryMap should be implemented in children core class"
    virtual UnknownType readMemory(unsigned long, unsigned long, unsigned char*, Segment);
    virtual UnknownType writeMemory(unsigned long, unsigned long, unsigned char const*, Segment);
    virtual int getIntProperty(int, unsigned long*, char const*);
    virtual UnknownType setIntProperty(int, unsigned long, char const*);
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*);
    virtual UnknownType setStringProperty(int, char*, unsigned long const*);
    virtual Model *getModel();
    virtual UnknownType addBreakpoint(Breakpoint*);
    virtual UnknownType removeBreakpoint(int);
    virtual UnknownType getBreakpoints(BPtype);
    virtual UnknownType test(int, Test*, int (*)(Model*));

protected:
    // in Model_core only
    virtual UnknownType core_init();
    virtual UnknownType core_config(char const*);
    virtual UnknownType core_reset(ResetType);
    virtual UnknownType core_isBreakInstr(unsigned int);
    virtual UnknownType core_getIntProperty(int, unsigned long*);
    virtual UnknownType core_setIntProperty(int, unsigned long);
};

class Avr8 : public Model_core
{
public:
    Avr8(Model_device*, int);
    ~Avr8();

    // inherited from Core
    virtual UnknownType writeMemory(unsigned long, unsigned long, unsigned char const*, Segment);
    virtual UnknownType peekReg(int, unsigned long*);
    virtual UnknownType pokeReg(int, unsigned long);
    virtual UnknownType readMemory(unsigned long, unsigned long, unsigned char*, Segment);

protected:
    // inherited from Model_core
    virtual UnknownType core_init();
    virtual UnknownType core_config(char const*);
    virtual UnknownType core_reset(ResetType);
    virtual UnknownType core_isBreakInstr(unsigned int);
    virtual UnknownType core_getIntProperty(int, unsigned long*);
    virtual UnknownType core_setIntProperty(int, unsigned long);
};

class Model_device : public Model
{
public:
    Model_device(char const*);
    ~Model_device();
    UnknownType dev_config(char const*);
    UnknownType dev_init(char const*);
    UnknownType dev_reset(ResetType);
    UnknownType getHaltReason();
    UnknownType step(int);

    UnknownType removePendingBreak(Breakpoint*);

    // order of field not known
    UnknownType m_timeStamp;

    // inherited from Model
    virtual Core *getCore(unsigned int);
    virtual UnknownType reset(ResetType);
    virtual UnknownType cycle(unsigned int);
    virtual UnknownType stop();
    virtual UnknownType addCycleCallback(void (*)(Model*, void*), void*);
    virtual UnknownType removeCycleCallback(int);
    virtual int getIntProperty(int, unsigned long*, char const*);
    virtual UnknownType setIntProperty(int, unsigned long, char const*);
    virtual UnknownType getStringProperty(int, unsigned long, char*, unsigned long const*);
    virtual UnknownType setStringProperty(int, char*, unsigned long const*);
    virtual UnknownType saveSim(SimState*);
    virtual UnknownType loadSim(SimState*);
    virtual int debug(char const*, int (*)(char const*, ...)); // implemented trivially : `return 0`
    virtual UnknownType test(int, Test*, int (*)(Model*));
};

class MemUnit
{
public:
    virtual ~MemUnit();

    virtual UnknownType getName() const = 0;
    virtual UnknownType isChanged(unsigned long, unsigned long) = 0;
    virtual UnknownType update(unsigned long, unsigned long) = 0;
    virtual UnknownType read(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> >&) = 0;
    virtual UnknownType write(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> > const&) = 0;
    virtual UnknownType getAddress() const = 0;
    virtual UnknownType getSize() const = 0;
};

class BasicMemUnit : public MemUnit
{
public:
    virtual ~BasicMemUnit();

    // inherited from MemUnit
    virtual UnknownType getName() const;
    virtual UnknownType isChanged(unsigned long, unsigned long);
    virtual UnknownType update(unsigned long, unsigned long);
    virtual UnknownType read(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> >&);
    virtual UnknownType write(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> > const&);
    virtual UnknownType getAddress() const;
    virtual UnknownType getSize() const;
};

class VerilogMemUnit : public MemUnit
{
public:
    virtual ~VerilogMemUnit();

    // inherited from BasicMemUnit
    virtual UnknownType getName() const;
    virtual UnknownType isChanged(unsigned long, unsigned long);
    virtual UnknownType update(unsigned long, unsigned long);
    virtual UnknownType read(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> >&);
    virtual UnknownType write(unsigned long, unsigned long, std::vector<unsigned char, std::allocator<unsigned char> > const&);
    virtual UnknownType getAddress() const;
    virtual UnknownType getSize() const;

    // VerilogMemUnit only
    virtual UnknownType getBits() const;
};

class MemFacade
{
public:
    MemFacade(model_verilated*, bool, int);
    virtual ~MemFacade();

    UnknownType write(unsigned int, unsigned int, unsigned char const*, unsigned int);
    UnknownType exists(unsigned int) const;
    UnknownType getMemUnit(unsigned int);
    UnknownType getMemUnit(std::string const&);
    UnknownType read(unsigned int, unsigned int, unsigned char*, unsigned int);
    UnknownType addMemUnit(unsigned int, MemUnit*);
    UnknownType addMemUnits(std::map<unsigned int, MemUnit*, std::less<unsigned int>, std::allocator<std::pair<unsigned int const, MemUnit*> > > const&);
};

#endif

