/*
 * config.h
 *
 *  Created on: Mar 18, 2012
 *      Author: pf
 */

#ifndef CONFIG_H_
#define CONFIG_H_

/* Begin parameters */

/* CPU */
#define CPU_FREQ        1 /* MHz*/

/* End parameters */

/* Duration of a single CPU cycle */
#define FCPU (CPU_FREQ*1000000)
#define CYCLE_DURATION (1000000000 / FCPU) /* nanoseconds */

#if !defined(__ASSEMBLER__)
#endif


#endif /* CONFIG_H_ */
