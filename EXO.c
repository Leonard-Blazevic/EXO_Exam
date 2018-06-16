/*since the desired periods are expressed with their max values, I created a sequence with periods:
1) cfrs => 200us
2) hsfods => 400us
3) mlmc => 400us
*/

void cfrs (void); // First task prototype (max period 200us)
void hsfods (void); // Second task prototype (max period 400us)
void mlmc (void); // Third task prototype (max period 800us)

void Timer_Init (void) // The interrupt is generated every 100us
{
    Stop_Timer();
    CNTREG = 0x0; // The timer starts from 0
    CMPREG = 0x1; // The interrupt will be generated every tick coming from the prescaler
    PRES = 0xFFFFFBC0; // 2^32 - 1088 => scales the frequency visible to the timer down to 10 kHz
    Start_Timer();
}

uint32_t Time = 0; // Global variable for task scheduling


void Timer_ISR () // Interrupt routine
{
    Clear_Interrupt_Flag();
    Time++;
    Task_Dispatcher();
}
                        //                              <-------400us------->
void Task_Dispatcher () // Task sequence => cfrs, hsfods, cfrs, mlmc, cfrs, hsfods, cfrs, mlmc
                       //                       <--200us-->          <---------400us------>
{
    Timer_Init(); // Timer reinitialization

    switch(Time) {
        case 1:
        case 3:
            cfrs(); // First task
            break;
        case 2:
            hsfods(); // Second task
            break;
        case 4:
            mlmc(); // Third task
            Time = 0; // Starting the sequence again
            break;
    }  
}

int Main () // The program starts here
{
    Timer_Init(); // Timer initialization and start of the counting

    while (1); // Infinite loop

    return 1;
}