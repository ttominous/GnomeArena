# Gnome Arena 0.3
-------------------
This is a hobby project, featuring an accumulation of code/know-how from the past few years.
A main goal of this project is to consolidate my logic into a core plugin that can be easily transferred between projects.

## Video Evidence:
April 2023 (Gameplay): https://youtu.be/5UQOFmziyTU

January 2023 (Retrospective/Preview): https://youtu.be/gpUVLuSM6_Q

## Packaged/Downloadable Game (very alpha!):
https://drive.google.com/file/d/1sA8seznZqT5ajv3OpnVXTkhvuI7Mv_Ob/view?usp=share_link

## UE5 Project:
https://drive.google.com/file/d/15dSEu6a9oXN1ksBGtZGmmog_reS5GEBR/view?usp=share_link

## Primarily In Code
- C++/Code based for the majority, with Blueprints used for data assets and animation blueprints.
- Majority of logic within COREPlay plugin, for easier porting of core gameplay and utilities between projects.
- Core plugin classes (GameInstance, GameMode, GameState, etc.) to manage matches and transitions between game stages (main menu, lobby, pre-game, in-game, post-game)
- Several file, math, UMG/UI and other utilities baked into the plugin

## Simple Networking/Multiplayer
- Login system using external site/SQL database (my own). Uses HTTP requests (JSON formatting
- User accounts and current/open matches are stored in the external DB.
- Simple match making allows player to quick join open matches (or host their own)
- Payload system to easily send structs of strings and numerics, consumed by Character or Action classes.

## UI System - Code + UMG
- Several helper methods coded into the plugin to easily query for and manipulate widget elements
- Several common widgets included in the plugin, including labeled/complex variations of base UE5 widgets
- Layered UI system via HUD class, allowing to easily switch between menus and general menu modes
- Main menu with lobby (level select and network player lists) as well as graphics settings and other pages/menus

## Action System for Tracking Animations and Attacks
- Action and ActionInstance classes with code to control action behavior (melee attacks, gun firing/reloading, etc.)
- Tracking of animations versus actions, with network replication to keep everyone in sync

## Inventory and Modular Bodies
- Core plugin has backed in classes and datassets for modular body parts and inventories (all replicated)

## Custom Assets
- Gnome models, animations, voices made my myself (Blender, GIMP, Audacity). Animations started in Mixamo, then heavily customized and fine tuned in Blender.
- In-Game Music made by friend, Jay Holm

## Complex Animations
- Layered animations (locomotion, hands, damage/hit, etc.) using custom AnimInstance logic + anim BP
- Walk, Run, Sprint, Crouch, Jump, Switch hands (left/right), melee (punch/swing as well as kick), shoot, equip weapon, reload
- Camera component to manage transitions between aiming, left/right hand switching and hide player when too close to camera
- FaceComponent and expression set assets, using morph shapes. Randomized micro expressions and blinking as well as hurt and dead face animations
