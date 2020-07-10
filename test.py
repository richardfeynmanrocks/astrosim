from mpl_toolkits import mplot3d
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import astrosim

scene = astrosim.Universe()
scene.add_star(7.34*(10**30), 10**6, [0,0,0], [0,0,0], [0,0,0], 10**26)
scene.add_matter(7.34*(10**30), 10**6, [0,0,0], [0,0,0], [0,0,0])
scene.add_matter(5.19*(10**25), 10**2, [0,9000,0], [-1,0,0], [0,0,0])
scene.add_photometer(10**2, [0,10000,0])

for i in range(10):
    print(i)
    scene.advance()

plt.plot(scene.photometers[0].recorded[1:])
plt.show()

#fig = plt.figure()
#ax = plt.axes(projection='3d')

#x1,x2=[]
#y1,y2=[]
#z1,z2=[]
#for j in range(1):
#    print("hi")
#    x1.append(scene.matter[0].position[0])
#    y1.append(scene.matter[0].position[1])
#    z1.append(scene.matter[0].position[2])
    #x.append(scene.stars[0].position[0])
    #y.append(scene.stars[0].position[1])
    #z.append(scene.stars[0].position[2])
#    scene.advance()
#    print("hi")
#ax.scatter3D(x1, y1, z1, label="Planet");
#ax.scatter3D(x2, y2, z2, label="Star");

#plt.legend()
#plt.show()
