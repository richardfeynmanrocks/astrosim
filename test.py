from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import astrosim

scene = astrosim.Universe()
scene.add_matter(7.34*(10**20), [3840000000,0,0], [0,100000000,0], [0,0,0])
scene.add_star(5.19*(10**25), [0,0,0], [1000000,1000000,0], [0,0,0], 10**26)

fig = plt.figure()
ax = plt.axes(projection='3d')

x=[]
y=[]
z=[]
for j in range(1000):
    x.append(scene.matter[0].position[0])
    y.append(scene.matter[0].position[1])
    z.append(scene.matter[0].position[2])
    scene.advance()
ax.scatter3D(x, y, z, label="Planet");

x=[]
y=[]
z=[]
for j in range(1000):
    x.append(scene.stars[0].position[0])
    y.append(scene.stars[0].position[1])
    z.append(scene.stars[0].position[2])
    scene.advance()
ax.scatter3D(x, y, z, label="Star");

plt.legend()
plt.show()
