import numpy as np
import matplotlib.pyplot as plot
import matplotlib.animation as animation

k = 2*np.pi
w = 2*np.pi
dt = 0.01

xmin = 0
xmax = 3
nbx = 100

x = np.linspace(xmin, xmax, nbx)

fig = plot.figure()
line, = plot.plot([], [])
line2, = plot.plot([],[])
print(line)
plot.xlim(xmin, xmax)
plot.ylim(-1, 1)


def init():
    line.set_data([],[])
    line2.set_data([],[])
    return line, line2,

def animate(i):
    t = i * dt
    y = np.cos(k*x - w*t)
    line.set_data(x, y)
    line2.set_data(y,x)
    return line, line2,


ani = animation.FuncAnimation(fig, animate, init_func=init, frames=100, blit=True, interval=20, repeat=False)
plot.show()