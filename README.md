# Timer-interrupt
The basic ideology behind this program is to run some code for a specific amount of time outside the main thread of execution, before returning to the main code once the time 
period has elapsed. This is done using the <b>timer/counter0</b> unit's Compare Match interrupts.<br><br>
The timer/counter0 is configured to generate a Compare Match interrupt every 1 millisecond. An infinite loop is also created within the <em>main</em> function to run as long as 
the variable "<em>waiting</em>" is 1. This will halt the normal execution of the program at the loop while the code within the ISR will run continuously. Once a specified time period in milliseconds has elapsed (given by the variable "<em>time</em>"), the variable "<em>waiting</em>" will be changed to 0 and the code will escape from the infinite loop and continue the normal routine of execution.<br><br>
Here the count is displayed in a 4-digit SSD.
