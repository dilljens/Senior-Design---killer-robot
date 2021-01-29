# -*- coding: utf-8 -*-
"""
Created on Sun Dec  6 13:02:51 2020

@author: Holden Bindl
"""

from gym.envs.registration import register
from gym import envs
all_envs = envs.registry.all()
env_ids = [env_spec.id for env_spec in all_envs]
desiredId = 'balancebot-v0'
if desiredId in env_ids:
    temp = None
else:
    register(id=desiredId,
        entry_point='TWSBR.envs:TWSBRenv',
        max_episode_steps = 5000,)