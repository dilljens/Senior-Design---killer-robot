# -*- coding: utf-8 -*-
"""
Created on Sun Dec  6 11:32:49 2020

@author: Holden Bindl, Dillon Jenson
"""
#imports
#physics engines
import gym
import pybullet
#python tools
import numpy as np
import itertools
import time
import datetime
#learning
import torch
from torch.utils.tensorboard import SummaryWriter
import TWSBR
from sac import SAC
from replay_memory import ReplayMemory
#create some variables that can be shared with ease
class Args:
    env_name = 'balancebot-v0'
    policy = 'Guassian'
    evalu = False
    gamma = 0.99
    tau = 0.005
    lr = 0.0003
    alpha = 0.2
    automatic_entropy_tuning = True
    seed = 123456
    batch_size = 256
    num_steps = 1000000
    hidden_size = 256
    updates_per_step = 1
    start_steps = 0
    update_interval = 1
    target_update_interval = 1
    replay_size = 1000000
    cuda = False
args = Args()
def main():
    env = gym.make(args.env_name)
    #reset the env
    env.reset()
    #seed the env
    env.seed(args.seed)
    torch.manual_seed(args.seed)
    np.random.seed(args.seed)
    agent = SAC(env.observation_space.shape[0],env.action_space,args)
    writer = SummaryWriter('runs/{}_SAC_{}_{}_{}'.format(datetime.datetime.now().strftime("%Y-%m-%d_%H-%M-%S"), args.env_name, args.policy, "autotune" if args.automatic_entropy_tuning else ""))
    memory = ReplayMemory(args.replay_size,args.seed)
    total_numsteps = 0
    updates = 0
    reward_history = []
    load = False
    save = False
    wdir = 'C:/Users/Dillon/OneDrive/Deep_Reinforcement_Learning/SendCode/models/sac_actor_TWSBR_env_'
    wdirt = 'models/sac_actor_TWSBR_env_'
    if load:
        agent.load_model(wdirt, wdirt)
        agent.eval()
        # pass
    #Begin training
    for i_episode in itertools.count(1):
        if save and i_episode % 10 == 0:
            agent.save_model('TWSBR_env')
            # pass
        e_reward = 0
        e_steps = 0
        done = False
        state = env.reset()
        while not done:
            if args.start_steps > total_numsteps:
                action = env.action_space.sample() #select a random action
            else:
                action = agent.select_action(state) #select an action from the policy
            if len(memory) > args.batch_size:
                for i in range(args.updates_per_step):
                    critic1_loss, critic2_loss, policy_loss, ent_loss, alpha = agent.update_parameters(memory,args.batch_size,updates)
                    writer.add_scalar('loss/critic_1', critic1_loss, updates)
                    writer.add_scalar('loss/critic_2', critic2_loss, updates)
                    writer.add_scalar('loss/policy', policy_loss, updates)
                    writer.add_scalar('loss/entropy_loss', ent_loss, updates)
                    writer.add_scalar('entropy_temprature/alpha', alpha, updates)
                    updates += 1
            next_state, reward, done, _ = env.step(action)
            e_steps += 1
            total_numsteps += 1
            e_reward += reward
            #print(next_state)
            mask = 1 if e_steps == 9000 else float(not done)
            memory.push(state,action,reward,next_state,mask) #append transistion to memory
            state = next_state
        if total_numsteps > args.num_steps:
            break
        reward_history.append(e_reward)
        avg_reward = np.mean(reward_history[-20:])
        writer.add_scalar('reward/train', e_reward, i_episode)
        print("Episode: {}, total numsteps: {}, episode steps: {}, reward: {}, average reward: {}".format(i_episode, total_numsteps, e_steps, round(e_reward, 2), round(avg_reward, 2)))
    env.render()
    done = False
    while 1:
            state = env.reset()
            while not done:
                action = agent.select_action(state)
                next_state, _, done, _ = env.step(action)
                state = next_state
    env.close()
    
if __name__ == '__main__':
    main()