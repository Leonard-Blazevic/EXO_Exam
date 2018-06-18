/*since the desired periods are expressed with their max values, I created a sequence with periods:
1) cfrs => 200us
2) hsfods => 400us
3) mlmc => 800us
*/

void cfrs (void); // First task prototype (max period 200us)
void hsfods (void); // Second task prototype (max period 400us)
void mlmc (void); // Third task prototype (max period 800us)

void Timer_Init (void) // The interrupt is generated every 100us
{
    Stop_Timer();
    CNTREG = 0x0; // The timer starts from 0
    CMPREG = 0x11; // 17(DEC) => The interrupt will be generated every 17. tick coming from the prescaler => output frequency of 10kHz
    PRES = 0x00000006; // 110 => address input of the multiplexer which sends the output of the 6. D bistable through,
                      // which gives a frequency fout = fin/2^6 = 170 kHz (10880 / 64 = 170)
    Start_Timer();
}

uint32_t Time = 0; // Global variable for task scheduling
int run_mlmc = 0; // Flag that is used for skipping the execution of the mlmc task in the middle of the cycle


void Timer_ISR () // Interrupt routine
{
    Clear_Interrupt_Flag();
    Time++;
    Task_Dispatcher();
}
                        //                              <-------400us------->
void Task_Dispatcher () // Task sequence => cfrs, hsfods, cfrs, EMPTY, cfrs, hsfods, cfrs, mlmc
                       //                       <--200us-->
                       //                   <-------------------------800us-------------------->
{
    Timer_Init(); // Timer reinitialization

    switch(Time) { // Since each task takes max. 90us to finish, and there is only one task per interrupt (100us), there shouldn't be any overlap
        case 1:
        case 3:
            cfrs(); // First task
            break;
        case 2:
            hsfods(); // Second task
            break;
        case 4:
            if (run_mlmc == 1) // Execute?
                mlmc(); // Third task
            
            run_mlmc = run_mlmc == 0 ? 1 : 0; // Invert the value of the flag
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