#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import gym
import tensorflow as tf
import tensorflow.contrib.slim as slim
import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
from tensorflow import keras
from ns3gym import ns3env

import time
import argparse
from ns3gym import ns3env
# from tcp_base import TcpTimeBased
# from tcp_newreno import TcpNewReno

__author__ = "Kenan and Sharif, Modified the code by Piotr Gawlowicz"
__copyright__ = "Copyright (c) 2020, Technische Universität Berlin"
__version__ = "0.1.0"
__email__ = "gawlowicz@tkn.tu-berlin.de"

parser = argparse.ArgumentParser(description='Start simulation script on/off')
parser.add_argument('--start',
                    type=int,
                    default=1,
                    help='Start ns-3 simulation script 0/1, Default: 1')
parser.add_argument('--iterations',
                    type=int,
                    default=1,
                    help='Number of iterations, Default: 1')
args = parser.parse_args()
startSim = bool(args.start)
iterationNum = int(args.iterations)

port = 5556
simTime = 10 # seconds
stepTime = 0.5  # seconds
seed = 12
simArgs = {"--duration": simTime,}
debug = False

env = ns3env.Ns3Env(port=port, stepTime=stepTime, startSim=startSim, simSeed=seed, simArgs=simArgs, debug=debug)
# simpler:
#env = ns3env.Ns3Env()
env.reset()

ob_space = env.observation_space
ac_space = env.action_space
mo_space = env.model_space

print("Observation space: ", ob_space,  ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)
print("Model space: ", mo_space, mo_space.dtype)

# def get_agent(obs):
#     print("hello IIIIIIIIIIIIIIIIIIII")
#     time.sleep
#     socketUuid = obs[0]
#     tcpEnvType = obs[1]
#     tcpAgent = get_agent.tcpAgents.get(socketUuid, None)
#     if tcpAgent is None:
#         if tcpEnvType == 0:
#             # event-based = 0
#             tcpAgent = TcpNewReno()
#         else:
#             # time-based = 1
#             tcpAgent = TcpTimeBased()
#         tcpAgent.set_spaces(get_agent.ob_space, get_agent.ac_space)
#         get_agent.tcpAgents[socketUuid] = tcpAgent

#     return tcpAgent

# # initialize variable
# get_agent.tcpAgents = {}
# get_agent.ob_space = ob_space
# get_agent.ac_space = ac_space

s_size = ob_space.shape[0]
print("State size: ",ob_space.shape[0])

a_size = 200
print("Action size: ", a_size)

model = keras.Sequential()
model.add(keras.layers.Dense(s_size, input_shape=(s_size,), activation='relu'))
model.add(keras.layers.Dense(s_size, input_shape=(s_size,), activation='relu'))
model.add(keras.layers.Dense(a_size, activation='softmax'))
model.compile(optimizer=tf.train.AdamOptimizer(0.001),
              loss='categorical_crossentropy',
              metrics=['accuracy'])

total_episodes = 16
max_env_steps = 5500
env._max_episode_steps = max_env_steps

epsilon = 1              # exploration rate
epsilon_min = 0.01
epsilon_decay = 0.999

packlost = []
packrate = []
packedelay = []
packaction = []

No_step = 0
reward = 0
done = False
info = None
RecvModel = True
# action_mapping = [2 ** i for i in range(a_size)]
action_mapping = [i for i in range(a_size)]
U_new =0
U =0
U_old=0
reward=0
#开始训练，可以训练的次数
for e in range(total_episodes):
    #重置环境
    obs = env.reset()
    obs = np.reshape(obs, [1, s_size])
    rewardsum = 0
    for mytime in range(max_env_steps):
        # Choose action
        if np.random.rand(1) < epsilon:
            action_index = np.random.randint(a_size)
            print (action_index)
            print("Value Initialization ...")
        else:
            action_index = np.argmax(model.predict(obs)[0])
            print(action_index)
        actions = [action_mapping[action_index]]
        next_state, reward, done, info = env.step(actions)
        print(next_state,reward,done,info)

        if info == "GetModel" :
            weights = model.get_weights()   
            for i, layer_weights in enumerate(weights):
                print("Layer {} weights shape: {}".format(i, layer_weights.shape))
                if i == 5:
                    isSend = env.send_model(layer_weights.tolist())
                    print(layer_weights)
                    print("marge : ",isSend)
                    RecvModel = False
                    while 0:
                        RecvModel = False
                    break
        
        if done:
            print("episode: {}/{}, time: {}, rew: {}, eps: {:.2}"
                  .format(e, total_episodes, time, rewardsum, epsilon))
            break
        # old的网络状态赋值
        U_old=0.9*(obs[0,0])+0.1*(obs[0,1])+0.3*(obs[0,2])

        next_state = np.reshape(next_state, [1, s_size])
                # 评估网络状态
        U_new=0.9*(next_state[0,0])+0.1*(next_state[0,1])+0.3*(next_state[0,2])
        
        U=U_new-U_old
        print("U is :",U)
        # 如果网络状态变差了 reward=-5 误差范围是0.05
        if U <-0.35:
            reward=10
        elif U >0.35:
            reward=-5
        else:
            reward=0
        # 执行下一步动作
        # Train
        target = reward
        if not done:
            target = (reward + 0.95 * np.amax(model.predict(next_state)[0]))
        # 这是输出所有的所以动作的概率
        target_f = model.predict(obs)
        # print("target :", target_f)

        target_f[0][action_index] = target
        # 模型训练
        model.fit(obs, target_f, epochs=1, verbose=0)
        # obs 设置成下一个状态
        obs = next_state
        rewardsum += reward 

        if epsilon > epsilon_min: epsilon *= epsilon_decay
        No_step += 1
  

        print("number of steps :", No_step)
        print("espsilon :",epsilon)

  
        print("reward sum", rewardsum)
        if obs[0,1] < 400:
            packlost.append(obs[0,0])
            packrate.append(obs[0,1])
            packedelay.append(obs[0,2])
            packaction.append(action_index)

    print("---------------------------------------------------------")
    weights = model.get_weights()   
    for i, layer_weights in enumerate(weights):
        print("Layer {} weights shape: {}".format(i, layer_weights.shape))
        print(layer_weights)
    print("---------------------------------------------------------")
        
    # print("Plot Learning Performance")
    # mpl.rcdefaults()
    # mpl.rcParams.update({'font.size': 16})
    # fig, ax = plt.subplots(figsize=(10,4))
    # plt.grid(True, linestyle='--')
    # plt.title('Learning Performance')
    # plt.plot(range(len(packlost)), packlost, label='packlost', marker="^", linestyle=":")#, color='red')
    # plt.plot(range(len(packrate)), packrate, label='packrate', marker="", linestyle="-")#, color='k')
    # plt.plot(range(len(packedelay)), packedelay, label='packedelay', marker="", linestyle="-"),# color='b')
    # plt.plot(range(len(packaction)),packaction, label='packaction', marker="", linestyle="-")#, color='y')
    # plt.xlabel('Episode')
    # plt.ylabel('Steps')
    # plt.legend(prop={'size': 12})
    # plt.savefig('learning.pdf', bbox_inches='tight')
    # plt.show() 
    print("Plot Learning Performance")
    mpl.rcdefaults()
    mpl.rcParams.update({'font.size': 16})
    fig, ax = plt.subplots(figsize=(10,4))
    plt.grid(True, linestyle='--')
    plt.title('Learning Performance')
    gap = 1
    plt.plot(range(0, len(packlost), gap), packlost[::gap], label='packlost', marker="^", linestyle=":")
    plt.plot(range(0, len(packrate), gap), packrate[::gap], label='packrate', marker="", linestyle="-")
    plt.plot(range(0, len(packedelay), gap), packedelay[::gap], label='packedelay', marker="", linestyle="-"),
    plt.plot(range(0, len(packaction), gap), packaction[::gap], label='packaction', marker="", linestyle="-")
    plt.xlabel('Episode')
    plt.ylabel('Steps')
    plt.legend(prop={'size': 12})
    plt.savefig('learning.pdf', bbox_inches='tight')
    plt.show()
    # print("Plot Learning Performance")
    # mpl.rcdefaults()
    # mpl.rcParams.update({'font.size': 16})
    # fig, axs = plt.subplots(2, 2, figsize=(10,8))
    # plt.suptitle('Learning Performance')

    # # First subplot
    # axs[0, 0].grid(True, linestyle='--')
    # axs[0, 0].set_title('packlost')
    # axs[0, 0].plot(range(len(packlost)), packlost, marker="^", linestyle=":")
    # axs[0, 0].set_xlabel('Episode')
    # axs[0, 0].set_ylabel('Steps')

    # # Second subplot
    # axs[0, 1].grid(True, linestyle='--')
    # axs[0, 1].set_title('packrate')
    # axs[0, 1].plot(range(len(packrate)), packrate, marker="", linestyle="-")
    # axs[0, 1].set_xlabel('Episode')
    # axs[0, 1].set_ylabel('Steps')

    # # Third subplot
    # axs[1, 0].grid(True, linestyle='--')
    # axs[1, 0].set_title('packedelay')
    # axs[1, 0].plot(range(len(packedelay)), packedelay, marker="", linestyle="-")
    # axs[1, 0].set_xlabel('Episode')
    # axs[1, 0].set_ylabel('Steps')

    # # Fourth subplot
    # axs[1, 1].grid(True, linestyle='--')
    # axs[1, 1].set_title('packaction')
    # axs[1, 1].plot(range(len(packaction)), packaction, marker="", linestyle="-")
    # axs[1, 1].set_xlabel('Episode')
    # axs[1, 1].set_ylabel('Steps')

    plt.tight_layout()
    plt.subplots_adjust(top=0.88)
    plt.savefig('learning.pdf', bbox_inches='tight')
    plt.show()

