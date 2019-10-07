#include "header.h"
#include "utils/math.h"
#include "logger.h"
#include "LibTool.h"

int main()
{
    LOG << helloWorld;
    LOG << utils::add(globalInt, 100);
    libTool();
    return 0;
}
