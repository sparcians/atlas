#include "core/STFtrace/STFLogger.hpp"

int main(){
    atlas::STFLogger test;
    test.initialize(true, 64, 0x1000);
    return 0;
}
