import numpy as np
import matplotlib.pyplot as plt

startingLabel = 2

naiveTimes = []
smartTimes = []
with open("output.txt", "r") as file:
    for line in file:
        line = line.strip()
        naiveTime, smartTime = line.split(",")
        naiveTimes.append(float(naiveTime))
        smartTimes.append(float(smartTime))

print(naiveTimes)
times = len(naiveTimes)
x_values = np.linspace(2, times+1, times)

plt.plot(x_values, np.array(naiveTimes), label='naive')
plt.plot(x_values, np.array(smartTimes), label='smart')
plt.yscale('log')
plt.xlabel('Edges Number')

labels = [str(int(i))+"N" for i in x_values]
for i in range(len(labels)):
    if i%5!=0:
        labels[i] = ""
        
plt.xticks(list(x_values), labels)

plt.ylabel('Milliseconds')
plt.title('Algorithms Comparison')
plt.legend()  # Show legend
plt.grid(True)  # Show grid
plt.show()