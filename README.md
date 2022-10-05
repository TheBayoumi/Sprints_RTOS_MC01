 ### Sprints_RTOS_MC01

Earliest Deadline First (EDF) is a shecduling algorithm that adopts a dynamic priority-based preemptive scheduling policy, meaning that the priority of a task can change during its execution, and the processing of any task is interrupted by a request for any higher priority task


>implemention an EDF scheduler using FreeRTOS

1. In the ""prvIdleTask"" function
   
      -Modify the idle task to keep it always the farest deadline

2. In the ""xTaskIncrementTick"" function:
   
      -In every tick increment, calculate the new task deadline and insert it in the correct position in the EDF ready list

3. In the ""xTaskIncrementTick"" function:
   
      -Make sure that as soon as a new task is available in the EDF ready list, a context switching should take place

>Verification of an EDF scheduler implementation

1. Using analytical methods calculate the following for the given set of tasks:

      
      -Calculate the system hyperperiod
      
      -Calculate the CPU load
      
      -Check system schedulability using URM and time demand analysis techniques (Assuming the given set of tasks are scheduled using a fixed priority rate monotonic scheduler)
  
2. Using Simso offline simulator

      -Fixed priority rate monotonic scheduler
  
3. Using Keil simulator in run-time

      -Calculate the CPU usage time



> Task List

- [x] Task 1: ""Button_1_Monitor"", {Periodicity: 50, Deadline: 50}

      -This task will monitor rising and falling edge on button 1 and send this event to the consumer task. (Note: The rising and failling edges are treated as separate events, hence they have separate strings)

- [x] Task 2: ""Button_2_Monitor"", {Periodicity: 50, Deadline: 50}

      -This task will monitor rising and falling edge on button 2 and send this event to the consumer task. (Note: The rising and failling edges are treated as separate events, hence they have separate strings)

- [x] Task 3: ""Periodic_Transmitter"", {Periodicity: 100, Deadline: 100}

      -This task will send preiodic string every 100ms to the consumer task

- [x] Task 4: ""Uart_Receiver"", {Periodicity: 20, Deadline: 20}

      -This is the consumer task which will write on UART any received string from other tasks

- [x] Task 5: ""Load_1_Simulation"", {Periodicity: 10, Deadline: 10}, Execution time: 5ms

- [x] Task 6: ""Load_2_Simulation"", {Periodicity: 100, Deadline: 100}, Execution time: 12ms

    
