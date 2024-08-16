import numpy as np 
import matplotlib.pyplot as plt 

#função logística
def logfunc(u):
    return u / (1. + np.abs(u))

#Ktz logístico
def KtzLog(x, y, z, k, H, I, T, d, l, x_R):
    x_next = logfunc((x - k * y + z + H + I)/T)
    y_next = x
    z_next = (1. - d)*z - l*(x - x_R)
    return x_next, y_next, z_next

#condições iniciais 
x_0 = 1. 
y_0 = 1.
z_0 = 1.

#parâmetros fixos
k = 0.6
H = 0.
I = 0.
d = 0.001
l = 0.001
T = 0.250
x_R = -0.200

#número de interações
n = 13000000

#tempo transiente
t = 2000

#lista para armazenar os resultados
x_values = []
y_values = []
z_values = []

x, y, z= x_0, y_0, z_0

for i in range(n):
    x, y, z = KtzLog(x, y, z, k, H, I, T, d, l, x_R)
    if i >= t:
        x_values.append(x)
        y_values.append(y)
        z_values.append(z)

plt.scatter(y_values, x_values, label='x', color='black', s=0.25)
plt.xlabel('x(t)')
plt.ylabel('x(t+1)')
plt.legend()
plt.savefig('diagramadeprimeiroretornopontos.png', dpi = 800)
