#include "header.h"
#include "utils/math.h"
#include "logger.h"
#include "LibTool.h"
#include "dummy.h"

int main()
{
    LOG << helloWorld;
    LOG << utils::add(globalInt, 100);
    libTool();
    dummyFunction();
    return 0;
}
