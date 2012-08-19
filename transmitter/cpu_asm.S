/*
 * Copyright (c) 2012 Paul Fleischer <paul@xpg.dk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
.global delayMs
.text

#include "config.h"
#if 1
/* If the inner-loop runs for 65535 x 3 cycles = 196605 cycles.
   At 1MHz this takes 196605/1MHz = 196ms.
   At 16MHz this takes 196605/16MHz = 12.28 ms.

   We want the inner-loop to run for as close to 1ms as possible.
   CYCLES = 1ms * CPU_FREQ.
   We subtract the overhead involved in controlling the inner-loop (2+1+2).
   The additional overhead for the return is not compensated, but is at most 1ms (@1MHz).
   Minimum delay by this function is 2ms @ 1MHz (when 1ms is given as argument).
*/
delayMs:
			mov #((FCPU/1000)-6)/3, r14		//	[2]
delayMs_inner:
			dec r14						//	[1]
			jne delayMs_inner			//	[2]
			dec r15						//	[1]
			jne delayMs					//	[2]
			ret							//	[3]

            .end

#endif
