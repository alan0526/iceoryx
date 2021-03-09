// Copyright (c) 2021 by Apex.AI Inc. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// SPDX-License-Identifier: Apache-2.0

#include "iceoryx_binding_c/publisher.h"
#include "iceoryx_binding_c/runtime.h"
#include "sleep_for.h"
#include "topic_data.h"

#include <signal.h>
#include <stdbool.h>
#include <stdio.h>

bool killswitch = false;

static void sigHandler(int f_sig)
{
    killswitch = true;
}

void sending()
{
    iox_runtime_init("iox-ex-callbacks-publisher");

    iox_pub_options_t options;
    iox_pub_options_init(&options);
    options.historyCapacity = 10U;
    options.nodeName = "iox-c-callbacks-publisher-node";

    iox_pub_storage_t publisherLeftStorage, publisherRightStorage;

    iox_pub_t publisherLeft = iox_pub_init(&publisherLeftStorage, "Radar", "FrontLeft", "Counter", &options);
    iox_pub_t publisherRight = iox_pub_init(&publisherRightStorage, "Radar", "FrontRight", "Counter", &options);

    for (uint32_t counter = 0U; !killswitch; ++counter)
    {
        struct CounterTopic* chunk;
        if (iox_pub_loan_chunk(publisherLeft, (void**)&chunk, sizeof(struct CounterTopic)) == AllocationResult_SUCCESS)
        {
            printf("Radar.FrontLeft.Counter sending  : %d\n", counter);
            chunk->counter = counter;
            iox_pub_publish_chunk(publisherLeft, chunk);
        }
        sleep_for(1000);

        if (iox_pub_loan_chunk(publisherLeft, (void**)&chunk, sizeof(struct CounterTopic)) == AllocationResult_SUCCESS)
        {
            printf("Radar.FrontLeft.Counter sending  : %d\n", counter + 10);
            chunk->counter = counter + 10;
            iox_pub_publish_chunk(publisherLeft, chunk);
        }
        sleep_for(1000);

        if (iox_pub_loan_chunk(publisherRight, (void**)&chunk, sizeof(struct CounterTopic)) == AllocationResult_SUCCESS)
        {
            printf("Radar.FrontLeft.Counter sending  : %d\n", counter * 2);
            chunk->counter = counter * 2;
            iox_pub_publish_chunk(publisherRight, chunk);
        }
        sleep_for(1000);
    }
}

int main()
{
    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    sending();

    return 0;
}
