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

    dlog("[UNIT_TEST] Beginning Tests .. \r\n");

    mem_ut();
    dlog("\r\n");

    ap_connect_ut();
    dlog("\r\n");
  
    screen_ut();
    dlog("\r\n");

    plot_ut();
    dlog("\r\n");
}
