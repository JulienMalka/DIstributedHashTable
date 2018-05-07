#include <stdio.h>
#include <stdlib.h>
#include "config.h"

#include "network.h"
#include "error.h"

int main(void)
{

    client_t client;
    client_init_args_t client_i;
    client_i.client = &client;
    client_i.name = "test";
    client_init(client_i);

    while(1)
        {

            char key_from1[MAX_MSG_SIZE];
            char key_from2[MAX_MSG_SIZE];
            char key_to[MAX_MSG_SIZE];
            int ok = 1;

            while (ok)
                {
                    int error = scanf("%s %s %s", key_from1, key_from2, key_to);
                    if (error != 1) ok = 0;
                    else
                        {
                            printf("FAIL\n");
                        }
                }

            /*get the values*/
            pps_value_t value_from1;
            pps_value_t value_from2;

            error_code error_get1 = network_get(client, key_from1, &value_from1);
            error_code error_get2 = network_get(client, key_from2, &value_from2);

            if (error_get1 != ERR_NONE || error_get2 != ERR_NONE)
                {
                    printf("FAIL\n");
                    continue;
                }

            /* concat the two values */
            char concat[MAX_MSG_SIZE];
            snprintf(concat, strlen(value_from1) + strlen(value_from2) + 1, "%s%s", value_from1, value_from2);

            /* re-send the concatenation */
            error_code error_put = network_put(client, key_to, concat);

            if (error_put != ERR_NONE)
                {
                    printf("FAIL\n");
                    continue;
                }

            printf("OK\n");
        }


}
