/* mbed Example Program
 * Copyright (c) 2006-2014 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mbed.h"
#include "mbed_events.h"

DigitalOut led1(LED1);
InterruptIn sw(USER_BUTTON);
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

// define the Serial object
Serial pc(USBTX, USBRX);

void rise_handler_user_context(void) {
    pc.printf("rise_handler_user_context in context %p\r\n", osThreadGetId());
    pc.printf("Blink! LED1 is now %d\r\n", led1.read());
}

void fall_handler_user_context(void) {
    pc.printf("fall_handler_user_context in context %p\r\n", osThreadGetId());
    pc.printf("Blink! LED1 is now %d\r\n", led1.read());
}

void rise_handler(void) {
    // Execute the time critical part first
    led1 = !led1;
    // The rest can execute later in user context (and can contain code that's not interrupt safe).
    // We use the 'queue.call' function to add an event (the call to 'rise_handler_user_context') to the queue.
    queue.call(rise_handler_user_context);
}

void fall_handler(void) {
    // Execute the time critical part first
    led1 = !led1;
    // The rest can execute later in user context (and can contain code that's not interrupt safe).
    // We use the 'queue.call' function to add an event (the call to 'fall_handler_user_context') to the queue.
    queue.call(fall_handler_user_context);
}

int main() {
    // Start the event queue
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    pc.printf("Starting in context %p\r\n", osThreadGetId());
    // The 'rise' handler will execute in interrupt context
    sw.rise(rise_handler);
    // The 'fall' handler will execute in interrupt context
    sw.fall(fall_handler);
}