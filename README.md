# Sprints_RTOS_MC01

Earliest Deadline First (EDF) is a shecduling algorithm that adopts a dynamic priority-based preemptive scheduling policy, meaning that the priority of a task can change during its execution, and the processing of any task is interrupted by a request for any higher priority task.


>Design EDF scheduler

Inorder for the EDF scheduler to work correctly, you still need to implement some changes that are not mentioned in the thesis:
"1. In the ""prvIdleTask"" function:

Modify the idle task to keep it always the farest deadline"
"2. In the ""xTaskIncrementTick"" function:

In every tick increment, calculate the new task deadline and insert it in the correct position in the EDF ready list"
"3. In the ""xTaskIncrementTick"" function:

Make sure that as soon as a new task is available in the EDF ready list, a context switching should take place. Modify preemption way as any task with sooner deadline must preempt task with larger deadline instead of priority"
    ### Task List

- [x] Write the press release
- [ ] Update the website
- [ ] Contact the media
    
