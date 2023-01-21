#include "test-global.h"
#include "variant-test.h"

int main ()
{
    init_variant();

    while (1){
        variant_service();
    }    

}