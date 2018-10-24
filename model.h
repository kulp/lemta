/* Inferred from libattiny*.so */

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

extern "C" int model_api_ver();
extern "C" Model_device *model_ctor(const char *);
extern "C" int model_dtor(Model_device *);

