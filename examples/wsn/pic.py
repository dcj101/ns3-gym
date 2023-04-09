import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

# 生成随机向量 a 和 b
a = np.random.rand(3)
b = np.random.rand(3)
print(a,b)
# 计算向量之间的夹角
theta = np.arccos(np.dot(a, b) / (np.linalg.norm(a) * np.linalg.norm(b)))
# print(a,b)

# 计算旋转轨迹
u = np.cross(a, b)
u /= np.linalg.norm(u)
rot_traj = np.array([a * np.cos(t * theta) + np.cross(u, a) * np.sin(t * theta) + u * np.dot(u, a) * (1 - np.cos(t * theta)) for t in np.linspace(0, 1, 100)])

# 计算偏移轨迹
offset_traj = np.array([a + np.dot(b - a, u) * u + np.linalg.norm(b - a) * np.sin(t * theta) * np.cross(u, b - a) / np.linalg.norm(np.cross(u, b - a)) for t in np.linspace(0, 1, 100)])

# 绘制图像
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.plot(rot_traj[:, 0], rot_traj[:, 1], rot_traj[:, 2], label='Rotation trajectory')
ax.plot(offset_traj[:, 0], offset_traj[:, 1], offset_traj[:, 2], label='Offset trajectory')
ax.scatter([a[0], b[0]], [a[1], b[1]], [a[2], b[2]], c=['r', 'g'], label=['a', 'b'])
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
ax.legend()
plt.show()
