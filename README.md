# Game Programming Projects

Welcome to my Game Programming Projects that have been produced by creating my own game engine! This repository contains a collection of game projects developed using the OpenGL Library in C++.

## Table of Contents
- [**Projects**](#Projects):
  - [Project 1: [Simple 2D Scene]](./simple_2d_scene)
  - [Project 2: [Pong Clone]](./pong_clone)
  - [Project 3: [Lunar Lander]](./lunar_lander)
- [**Exercises**](#Exercises):
  - [Exercise 1: [Player Input]](./player_input)
  - [Exercise 2: [Animation]](./animation)

## Overview

This repository serves as a showcase of my game programming projects from CS3113 - Introduction to Game Programming. Each project is housed in its respective directory, containing source code, assets, and documentation.

## _Projects_

### [Project 1: Simple 2D Scene](./simple_2d_scene)

This first project encapsulates basic concepts and establishes groundwork for future projects. Topics explored in this project includes: delta time, matrix translation, matrix rotation, and matrix scaling. The two sprites are from a video game call **Stardew Valley**, it is one of my favorite games to play.
  
<img width="600" alt="stardew_spin" src="https://github.com/jaylan-wu/CS3113/assets/89537744/f496d6ed-29e4-43f6-8dbb-24cbd361c461">

### [Project 2: Pong Clone](./pong_clone)

This second project incorporates two important topics in game engineering and design: user input and collision. To practice with these concepts, this assigment asked the programmers to imitate the classic game of pong. To style it into my own version of pong, I designed the game around one of my favorite music groups, **LE SSERAFIM**. Some features in this game include having 1 Player and 2 Player game modes by pressing t and having up to 3 balls in play by pressing 1, 2, or 3.

<img width="600" alt="lsrfm_pong" src="https://github.com/jaylan-wu/CS3113/assets/89537744/9b0c6733-c0b3-4697-96c5-64cc1c45a2d5">

### [Project 3: Lunar Lander](./lunar_lander)

In this project, we have now created an entity class that allows us to create more concise code without repeating commonly used functions. With this, we are able to create a large number of entities that we can then interact with. By applying this new class, we were assigned to attempt a recreation of the 1979 version of Lunar Lander by Atari. Despite not have all the functions of the original game, I decided to design my version of lunar lander around the music video New Jeans by NewJeans. To play the game, the player has to use the left and right arrow keys to move the character around while trying to land on one of the purple buildings in the foreground. In addition, there is a power system in which if the player moves a certain amount, the power level will decrease. If the power level is depleted all the way, the user may not move anymore.

<img width="600" alt="nwjns_lunar_lander" src="https://github.com/jaylan-wu/CS3113/assets/89537744/ea4bf39c-653a-444e-955a-4ea7946d8011">

## _Exercises_

### [Exercise 1: Player Input](./player_input)

This first exercise establishes player input through the keyboard. This allows the up, down, left, and right keys to move a sprite along the screen. By pressing r, we can rotate the sprite clockwise along the z-axis, if c is pressed, the sprite would then rotate counter-clockwise, and pressing s would stop any rotation along the z-axis.

### [Exercise 2: Animation](./animation)

This exercise establishes an understanding of animation through the use of a sprite sheet. By having a sprite sheet of 4 sonic characters, the sprites on the screen rotate through these 4 characters until the user presses the s button. This would stop the rotation through each character and one character would be chosen.

