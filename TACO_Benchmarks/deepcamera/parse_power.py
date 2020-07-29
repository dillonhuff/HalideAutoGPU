#!/usr/bin/python3

import statistics

lines = open('times.csv', 'r').readlines()
assert(len(lines) == 2)

start = int(lines[0])
end = int(lines[1])

print('start = ', start)
print('end   = ', end)

power_log = open('power.csv', 'r').readlines()
print('power logs =', len(power_log))

power_readings = []
for l in power_log:
    fields = l.split(', ')
    time = int(fields[2])
    power = int(fields[3])
    if (start < time and time < end):
        print('power:', power)
        power_readings.append(power)

average_power = statistics.mean(power_readings)
print('Average power =', average_power)

