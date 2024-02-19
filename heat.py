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

heatmap, xedges, yedges = np.histogram2d(x, y, bins=50)

plt.imshow(heatmap.T, extent=[xedges[0], xedges[-1], yedges[0], yedges[-1]], origin='lower')
plt.colorbar(label='Counts')
plt.xlabel('Coreness')
plt.ylabel('Sum of first 3 densities')
plt.title('Heatmap')
plt.show()