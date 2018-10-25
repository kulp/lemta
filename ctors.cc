#define _FORTIFY_SOURCE 1

#include "model.hh"

int main()
{
    Model_device *pmd = new Model_device("attiny1616");
    // valgrind detects heap smashing
    pmd->step(3);

    Model_device md("attiny1616");
    // G++ detects stack smashing
    md.step(3);
}

