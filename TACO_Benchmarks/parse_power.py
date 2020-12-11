#!/usr/bin/python3

import statistics

lines = open('times.csv', 'r').readlines()
assert(len(lines) == 3)

start = int(lines[0])
end = int(lines[1])
runs = int(lines[2])

print('start = ', start)
print('end   = ', end)
print('runs  = ', runs)

power_log = open('power.csv', 'r').readlines()
print('power logs =', len(power_log))

power_readings = []
for l in power_log:
    print('Line:', l)
    fields = l.split(', ')
    if len(fields) > 3:
        time = int(fields[2])
        power = int(fields[3])
        if (start < time and time < end):
            print('power:', power)
            power_readings.append(power)

average_power = statistics.mean(power_readings)
print('Average power =', average_power, 'W')

duration_us = end - start
print('Duration      =', duration_us, ' usec')
duration = duration_us / 1000000.0
print('Duration      =', duration, '  sec')

pixels_per_image = 2048*2048
print('Pixels per img=', pixels_per_image)
pixels_in = pixels_per_image*runs
throughput = pixels_in / float(duration)

power_perf = throughput / float(average_power)
power_perf = power_perf / 1000000000.0

throughput_gp = throughput / 1000000000.0
print('Throughput    =', throughput_gp, ' GPix / sec')
print('Power perf    =', power_perf, ' GPix / J')

