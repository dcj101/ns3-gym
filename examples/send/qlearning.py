import ns3gym
import numpy as np

# 创建一个Ns3Env对象
env = ns3gym.Ns3Env(port=5555, stepTime=0.5, startSim=True, simSeed=0, simArgs={"--duration": 10}, debug=False)

# 获取观察空间和动作空间
obs_space = env.observation_space
act_space = env.action_space

# 定义一个简单的强化学习算法
def q_learning(env, num_episodes=1000, alpha=0.5, gamma=0.9, epsilon=0.1):
    # 初始化Q表
    Q = np.zeros((obs_space.n, act_space.n))

    # 迭代训练
    for i in range(num_episodes):
        # 重置环境
        obs = env.reset()

        # 选择动作
        if np.random.uniform() < epsilon:
            action = act_space.sample()
        else:
            action = np.argmax(Q[obs])

        # 执行动作并观察结果
        obs_next, reward, done, info = env.step(action)

        # 更新Q表
        Q[obs, action] += alpha * (reward + gamma * np.max(Q[obs_next]) - Q[obs, action])

        # 更新状态
        obs = obs_next

        # 如果达到终止状态，重置环境
        if done:
            obs = env.reset()

    return Q

# 训练强化学习算法
Q = q_learning(env)

# 关闭环境
env.close()