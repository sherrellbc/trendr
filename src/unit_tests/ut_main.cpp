#include "unit_tests.h"
#include "logging.h"


void do_unit_tests(uint32_t which){
//    switch(which){
//        case which & AP_UTS:
//            ap_connect_uts();
//            break;
//    
//        default:
//            break;
//    }

    mem_ut();
    ap_connect_ut();
    screen_ut();
}
