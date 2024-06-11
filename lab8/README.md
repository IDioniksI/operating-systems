## Synchronize Threads
[The first task was](lab8.cpp): Simulate the operation of the crossing, creating a separate flow for each soldier. The program must synchronously (that is, in the same order as it happened) write in the log the events of the arrival of soldiers, their boarding on the ferry, departure and return of the ferry, with time stamps and a report of how many soldiers are in line and how many are on the ferry. All soldiers of the same type or 50/50 can ride on the ferry. The ferry only departs when it is fully booked.
<br>[The second task was](lab8_pi.cpp): Implement a multithreaded calculation of the value of the number π using the Leibniz series. Each thread calculates a part of the sum, and then these partial sums are combined to produce the final result