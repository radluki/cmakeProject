#include <algorithm>
#include "logger.h"

#include <iostream>
#include <string>

class Field
{
public:
    Field()
    {
        LOG << "Field ctr";
    }

    ~Field()
    {
        LOG << "Field dtr";
    }
};

class Parent
{
public:
    Parent()
    {
        LOG << "Parent ctr";
    }

    ~Parent()
    {
        LOG << "Parent dtr";
    }
};

class Thrower : public Parent
{
    Field f;

public:
    Thrower()
    {
        LOG << "Thrower ctr - throwing exception";
        throw std::runtime_error("thrown by Thrower");
    }

    ~Thrower()
    {
        LOG << "Thrower Destructor";
    }
};

int main()
try
{
    Thrower t;
}
catch (const std::exception &e)
{
    LOG << "Caught exception: " << e.what();
}