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

port = 5555
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
# mo_space = env.model_space

print("Observation space: ", ob_space,  ob_space.dtype)
print("Action space: ", ac_space, ac_space.dtype)
# print("Model space: ", mo_space, mo_space.dtype)

s_size = ob_space.shape[0]
print("State size: ",ob_space.shape[0])

a_size = 16
print("Action size: ", a_size)

model = keras.Sequential()
model.add(keras.layers.Dense(s_size, input_shape=(s_size,), activation='relu'))
model.add(keras.layers.Dense(s_size, input_shape=(s_size,), activation='relu'))
model.add(keras.layers.Dense(a_size, activation='softmax'))
model.compile(optimizer=tf.train.AdamOptimizer(0.0001),
              loss='categorical_crossentropy',
              metrics=['accuracy'])

total_episodes = 16
max_env_steps = 155000
env._max_episode_steps = max_env_steps

epsilon = 1              # exploration rate
epsilon_min = 0.001
epsilon_decay = 0.9992
# epsilon_decay = 1

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
                    print(len(layer_weights.tolist()))
                    isSend = env.send_model(layer_weights.tolist())
                    print(layer_weights)
                    print("marge : ",isSend)
                    np.reshape(isSend,[1,a_size])
                    weights[5] = isSend
                    model.set_weights(weights)
                    RecvModel = False
                    while 0:
                        RecvModel = False
                    break
        
        if done:
            break

        U_old=0.92*(np.log(obs[0,0]))-0.1*(np.log(obs[0,1]))+0.3*(obs[0,2])
        next_state = np.reshape(next_state, [1, s_size])
        U_new=0.92*(np.log(next_state[0,0]))-0.1*(np.log(next_state[0,1]))+0.3*(next_state[0,2]) 
        U=U_new-U_old
        print("U is :",U)
        if U <-0.01:
            reward=6
        elif U >0.0001:
            reward=-5
        else:
            reward=0
        # 执行下一步动作
        # Train
        target = reward
        if not done:
            target = (reward + 0.98 * np.amax(model.predict(next_state)[0]))
        print("tag is ",target)
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
        if obs[0,1] < 10000 and obs[0,1] > 0:
            packlost.append(obs[0,0])
            packrate.append(obs[0,1])
            packedelay.append(obs[0,2])
            # packaction.append(action_index*10)

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
    
    print("Plot Learning Performance wsn0")
    mpl.rcdefaults()
    mpl.rcParams.update({'font.size': 16})
    fig, ax = plt.subplots(figsize=(10,4))
    plt.grid(True, linestyle='--')
    plt.title('Learning Performance')
    gap = 15
    plt.plot(range(0, len(packlost), gap), packlost[::gap], label='packetlost', marker="^", linestyle=":")
    plt.plot(range(0, len(packrate), gap), packrate[::gap], label='packetrate', marker="", linestyle="-")
    plt.plot(range(0, len(packedelay), gap), packedelay[::gap], label='packetdelay', marker="", linestyle="-"),
    plt.plot(range(0, len(packaction), gap), packaction[::gap], label='packetaction', marker="", linestyle="-")
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

    # plt.tight_layout()
    # plt.subplots_adjust(top=0.88)
    # plt.savefig('learning.pdf', bbox_inches='tight')
    # plt.show()

