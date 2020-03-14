#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Mar 11 11:54:42 2020

@author: stud
"""
import sys
import os

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))




from Smartsoundsystem import gui



func = raw_input("Type which gui class you want info about\n")

funcclass = getattr(gui, func)

help(funcclass)

    




