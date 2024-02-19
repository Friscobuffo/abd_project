import numpy as np
import matplotlib.pyplot as plt

startingLabel = 2

naiveTimes = []
smartTimes = []
with open("output2.txt", "r") as file:
    for line in file:
        line = line.strip()
        naiveTime, smartTime = line.split(",")
        naiveTimes.append(float(naiveTime))
        smartTimes.append(float(smartTime))

naiveTimes = list(reversed(naiveTimes))
smartTimes = list(reversed(smartTimes))

times = len(naiveTimes)
x_values = np.linspace(0, times+1, times)

plt.plot(x_values, np.array(naiveTimes), label='naive')
plt.plot(x_values, np.array(smartTimes), label='smart')

plt.xlabel('Biconnected Components Number')

labels = [1, 2, 5, 10]
        
plt.xticks(list(x_values), labels)

plt.ylabel('Milliseconds')
plt.title('Algorithms Comparison')
plt.legend()  # Show legend
plt.grid(True)  # Show grid
plt.show()