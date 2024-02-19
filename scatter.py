import matplotlib.pyplot as plt
import numpy as np

x = []
y = []
with open("output3.txt", "r") as file:
    for line in file:
        line = line.strip()
        a, b = line.split(",")
        x.append(float(b))
        y.append(float(a))

x = np.array(x)
y = np.array(y)

plt.scatter(x, y, color='blue', alpha=0.5)


plt.xlabel('Coreness')
plt.ylabel('Sum of first 3 densities')
plt.title('Scatter')

plt.grid(True)
plt.show()