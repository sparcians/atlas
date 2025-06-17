#include "core/observers/STFtrace/STFLogger.hpp"

int main(){
    atlas::STFLogger test(32, 0x1000, "Test_enable.stf");
    atlas::STFLogger test(32, 0x1000, ""); //disable STF trace generation
    return 0;
}
