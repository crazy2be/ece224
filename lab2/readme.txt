Readme - hello_alt_main Software Example

DESCRIPTION:	
Demonstration of a "Freestanding" Nios II application.

PERIPHERALS USED:
This example exercises the following peripherals:
- STDOUT device (UART or JTAG UART)

SOFTWARE SOURCE FILES:
This example includes the following software source files:
- hello_alt_main.c: Demonstration of a free-standing application that uses
STDOUT device.

BOARD/HOST REQUIREMENTS:
This example requires only a JTAG connection with a Nios Development board. If
the host communication settings are changed from JTAG UART (default) to use a
conventional UART, a serial cable between board DB-9 connector  and the host is
required.

ADDITIONAL INFORMATION:
hello_alt_main: The classic K&R simplest-possible-program, adapted to be a
"Freestanding" C program. This program prints the string "Hello from Nios II
Free-Standing" to the STDOUT stream.

According to the ANSI C standard, freestanding programs "own" the hardware, and
cannot rely on system-services or device-drivers being initialized prior to
program-start. A freestanding program is responsible for initializing all
hardware devices, device-drivers, and system-services. Many embedded programs
are, by nature, freestanding. The author relinquishes any illusion of running
their program on a workstation.

You create a freestanding Nios II program by defining the function:

   void alt_main (void)

As opposed to main() as a "hosted" application would (see the Hello World
example for "hosted hello world").

To ease debugging when the debugger is set to automatically break at "main()",
this program still defines the symbol "main()" with the statement:
- int main (void) __attribute__ ((weak, alias ("alt_main")));
The Nios II Software Build Tools debugger can also be set to break at "alt_main()",in which 
case the above statement would be unneccessary.

Upon entry to alt_main():
- The CPU's caches (if any) have been initialized.
- The stack-pointer has been set.
- That's all. The rest is up to you.

If you want to write a program that gets in even earlier, you will need to
provide your own assembly language machine-setup code by defining the symbol
"_start". Any definition of _start in your directory will override the BSP
definition. You can find source code for the Nios II BSP _start here:

  <NiosII-Kit-Install-Dir>/components/altera_nios2/HAL/src/crt0.S

What this Software Example Illustrates:
The hello_alt_main example does nothing special; in fact, it does exactly what the
"hosted" edition of Hello World would do, and subsequently uses approximately
the same amount of memory. The difference is that each bit of the system is
brought to life within the control of the application developer. This has
intrinsic benefits AND dangers as the unsuspecting user can more easily make
mistakes which may render the system useless, or only partially functional. It
should be noted that modifications made at this level of development are done
"at your own risk, no lifeguard on duty".

Reducing Memory Footprint in Free-Standing Nios II Applications:
So you want to take the free-standing application plunge, do you? First, note
that the HAL environment released with Nios II, in conjunction with Nios II Software Build Tools,
lets you reduce PLENTY of memory overhead in various GUI-options. In fact, you
can make a hosted application nearly as small as a free-standing one by changing
GUI options alone. Refer to the Nios II Software Developer's Handbook for
details.

However, the developer that wants or needs a free-standing application should be
aware of a few tips and tricks for reducing memory footprint (if desired): 

1. Turn on compiler optimization to -03 for both your application and BSP project.

2. Replace the alt_sys_init() call with your own custom system initialization
routine that initializes device drivers for ONLY the peripherals that you need
them for; this is useful if you want to explicitly control a peripheral via its
registers, or if you don't wish to use the Altera supplied-and-tested HAL
drivers in lieu of your own. To do this, simply comment out the alt_sys_init()
call and create your own that looks like the following example. In this code
excerpt, only the HAL driver for the Altera Avalon UART named "uart1" is called:

#include "system.h"
#include "sys/alt_sys_init.h"
#include "altera_avalon_uart.h"

/*
 * Allocate the device storage
 */
ALTERA_AVALON_UART_INSTANCE( UART1, uart1 );


/*
 * Initialize the device(s)
 */
void custom_sys_init( void )
{
  ALTERA_AVALON_UART_INIT( UART1, uart1 );
}

This system initialization code can be explicitly placed in your own custom
small_alt_main() routine, or saved cleanly in a separate source file.

3. If you are still using any of the Altera HAL drivers, some are designed with
size in mind. The drivers can be conditionally compiled to reduce memory
foot-print. This is controlled via the Nios II Software Build Tools for Eclipse in the
"properties" of your board support package (BSP). Check the "use small footprint drivers"
option to select this. Please refer to the Nios II Software Developers Handbook for more
detail on small footprint drivers.

4. Select the "small C library" option for your BSP in the properties
(same location as small footprint drivers in step 3).

5. If your application doesn't require interrupts, and/or doesn't need a clean
"exit", comment out the alt_irq_init() and exit() calls from your
alt_main()-based application.

6. Disable Instruction Emulation
The HAL software exception handler includes the ability to emulate multiply and
divide instructions when they are not supported by the processor. This feature
can be disabled by defining the C preprocessor macro: ALT_NO_INSTRUCTION_EMULATION
for the BSP project. This is safe to do this if you are using a core 
which supports hardware multiply/divide. In most cases, this is also safe to do 
even if your processor does not support hardware multiply/divide. BSP 
projects and application projects built for systems which do not have support for 
hardware multiple/divide instructions, will be compiled and linked with the
-mno-hw-mul option.  Therefore code compiled as a part of these projects does not 
require multiply instruction emulation. Divide instruction emulation is only required
if you explicitly compile your code with the –mhw-div option.


