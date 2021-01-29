# -*- coding: utf-8 -*-
"""
Created on Sun Dec  6 11:46:39 2020

@author: Holden Bindl, Dillon Jensen
"""
#imports
import os
import math
import numpy as np

import gym
from gym import spaces
from gym.utils import seeding

import pybullet as p
import pybullet_data

posx = 0
posy = 0

class TWSBRenv(gym.Env):
    def __init__(self, render=True):
        self.observation = []
        self.action_space = spaces.Box(np.array([-2, -2], dtype=np.float32),np.array([2, 2], dtype=np.float32))
        self.observation_space = spaces.Box(np.array([-math.pi, -math.pi,-5,-5,-10,-10,-10,-10]), np.array([math.pi, math.pi,5,5,10,10,10,10])) # pitch, gyro, com.sp.
        #define max episode steps in the init
        self.balance = True
        self.speed_def = 0.015
        if(render):
            self.client = p.connect(p.GUI)
        else:
            self.client = p.connect(p.DIRECT)
        p.setAdditionalSearchPath(pybullet_data.getDataPath())
        self.seed()
        self.reset()
        self.planeID = p.loadURDF("plane.urdf",basePosition=[0, 0, 0], physicsClientId=self.client)
        
    def seed(self, Seed = None):
        self.np_random, Seed = seeding.np_random(Seed)
        return [Seed]
    def reset(self):
        self.vt = [0,0]
        self.vd = 0
        self.envStepCounter = 0
        p.resetSimulation()
        p.setGravity(0,0,-10)
        p.setTimeStep(0.01)
        planeID = p.loadURDF("plane.urdf",basePosition=[0, 0, 0], physicsClientId=self.client)
        cubeStartPos = [0,0,.18]
        cubeStartOrientation = p.getQuaternionFromEuler([0,0,0]) #could be randomized
        path = os.path.abspath(os.path.dirname(__file__))
        self.botId = p.loadURDF(os.path.join(path, "balancebot_simple.xml"),cubeStartPos,cubeStartOrientation)
        
        
        #Random goal
        x = (self.np_random.uniform(2, 4) if self.np_random.randint(2) else
             self.np_random.uniform(-2, -4))
        y = (self.np_random.uniform(2, 4) if self.np_random.randint(2) else
             self.np_random.uniform(-2, -4))
        self.goal = [x, y]
        GoalID = p.loadURDF(os.path.join(path, "simplegoal.urdf"),basePosition=[self.goal[0],self.goal[1],0],physicsClientId=self.client)
        self.prev_dist_to_goal = math.sqrt((self.goal[0] ** 2 + self.goal[1] ** 2))
        #get the first observation and return
        self.observation = self.compute_observation()
        return self.observation
    def step(self,action):
        self.vt = self.vt+action
        p.setJointMotorControl2(self.botId,jointIndex=0,controlMode=p.VELOCITY_CONTROL,targetVelocity=self.vt[0])
        p.setJointMotorControl2(self.botId,jointIndex=1,controlMode=p.VELOCITY_CONTROL,targetVelocity=self.vt[1])
        p.stepSimulation()
        self.envStepCounter += 1
        if self.envStepCounter>=500:
            self.balance = True
            #self.speed_def = 0
        self.observation = self.compute_observation()
        return self.observation, self.reward(), self.done(), {}
    def compute_observation(self):
        Pos, Orn = p.getBasePositionAndOrientation(self.botId,self.client)
        Euler = p.getEulerFromQuaternion(Orn)
        linear, angular = p.getBaseVelocity(self.botId,self.client)
        return np.array([Euler[0],angular[0],self.vt[0],self.vt[1],self.goal[0],self.goal[1],Pos[0],Pos[1]],dtype=np.float32)
    def reward(self):
     
        Pos, Orn = p.getBasePositionAndOrientation(self.botId,self.client)
        Euler = p.getEulerFromQuaternion(Orn)
        dist_to_goal = math.sqrt((Pos[0]-self.goal[0])**2+(Pos[1]-self.goal[1])**2)
        linear, angular = p.getBaseVelocity(self.botId,self.client)
        goal_reward = 0
        if self.balance:
            if (dist_to_goal>self.prev_dist_to_goal):
                goal_reward = -2 # Having -2 here prevents spinning in place
            else:
                goal_reward = 1
            if (dist_to_goal<0.5):
                goal_reward = 1500 # Works fine with 500 but I saw the robot would get more reward for not reaching the goal sometimes
                print("win")
                
        dy = self.goal[1] - Pos[1]
        dx = self.goal[0] - Pos[0]
        
        global posy
        global posx
        pos1 = posy
        pos0 = posx
        posy = Pos[1]
        posx = Pos[0]      
        vel_angle = np.arctan2(posy-pos1,posx-pos0)
        #r_angle = math.fmod(angular[2], 2*math.pi)
        goal_angle = np.arctan2(dy, dx)
        angle_diff = goal_angle - vel_angle
        #print(angle_diff * 180 / math.pi, vel_angle * 180 / math.pi , math.cos(angle_diff)**2)
        self.prev_dist_to_goal = dist_to_goal
        linear = math.sqrt(linear[1]**2 + linear[0]**2)
        #try adding robot speed
        angularr = abs(angular[0])+abs(angular[1])+abs(angular[2])
        goal_balance = abs(Euler[0])
        #speed = abs(self.vt[0])+abs(self.vt[1])
        if goal_balance<.8:
            goal_balance = 1
        else:
            goal_balance = -1.5
        tilt_goal = 0
        ang_diff = Euler[0] - tilt_goal 
        
        if abs(ang_diff)<.7:
            bal_reward = 1.5*math.cos(ang_diff)
        else:
            bal_reward = -1.5
        
        if abs(angular[0])<.5:
            ang0 = 1
        else:
            ang0 = -abs(angular[0])
            
        if abs(angular[1])<.1:
            ang1 = 1
        else:
            ang1 = -abs(angular[1])
        
        if abs(angular[2])<.1:
            ang2 = 1
        else:
            ang2 = -abs(angular[2])
            
        if linear<.5:
            lin = 1
        else:
            lin = -linear
        # ang0 = .2 -abs(angular[0])
        # ang1 = .2 -abs(angular[1])
        # ang2 = .2 -abs(angular[2])
        # lin = .2 - linear
        #print(angular[0],angular[1],angular[2])
        cos_diff = round(math.cos(angle_diff)**4)
        #return 0.025*goal_balance + 0.5*goal_reward*(cos_diff) - abs(angular[2])*self.speed_def
        # return (0.025*goal_balance + 0.5*goal_reward - abs(angular[2])*self.speed_def)*1
        return 0.5*bal_reward + 0.01*(ang0 + ang1 + ang2 + lin)
    
    def done(self):
        Pos, Orn = p.getBasePositionAndOrientation(self.botId,self.client)
        #Euler = p.getEulerFromQuaternion(Orn)
        Done = False
        if (Pos[2] < 0.25):
            Done = True
        elif (Pos[2] > .75):
            Done = True
        elif (self.envStepCounter>=2500):
            Done = True
        elif (self.prev_dist_to_goal<0.5):
            Done = True
        return Done
    def render(self,mode='human',close=False):
        self.close()
        self.__init__(render = True)
        pass
    def close(self):
        p.disconnect(self.client)
