# 🔗 UEOS_GameInstance — Epic Online Services Integration for Kingdoms Edge

This Unreal Engine 5 class (UEOS_GameInstance) provides a full Blueprint-accessible interface for Epic Online Services (EOS), designed for multiplayer games like Kingdoms Edge. It acts as a centralized hub for handling EOS login, session creation, matchmaking, party management, friend systems, and user data storage.

*Note* : This should be copied in the source forlder of your project and has therefore no file organisation here.
## 🎯 Features

- Authentication & Identity

   - Login/logout players
   -  Retrieve player nickname, UniqueNetId, and account data

- Session Management

   - Create, join, destroy EOS sessions
   - Search for sessions or friends' sessions
   - Handle session state and settings

- Party System

   - Create, join, leave, and manage parties
   - Promote/kick members, send/cancel invitations
   - Access real-time updates for party events

- Friends System

    -   Read friend list
    -  Check online status & presence info
    - Respond to friend list changes

- Presence
   - Query and cache player presence data
   - Check if friends are online, in-game, or joinable

 - User File Storage

   -  Enumerate, read, write, and delete user files on EOS cloud

## 🧩 Blueprint Integration

The system is fully exposed to Blueprints, making it easy to:

-    Build multiplayer menus

 -   Display friend lists and invite buttons

  -  Track player status and session transitions

Each functionality has a UFUNCTION(BlueprintCallable) counterpart and is supported by well-structured USTRUCT types (e.g., FTamBPSessionSettings, FTamBPOnlineFriend).
## 🛠️ Dependencies

This class uses Epic Online Services (EOS) via the Redpoint EOS Online Subsystem Plugin and must be properly configured in the project settings with:

-    AuthGraph setup

 -   EOS credentials

  -  Plugin path (OnlineLobbyInterface macro must match your platform)

## 🔄 Delegates

A wide set of FDelegate... types are included to support async operations like login completion, session search results, party invites, and more. These allow clean Blueprint workflows without binding custom C++ logic.
📁 File Location

/Source/Kingdoms_Edge/Public/EOS_Gameinstance.h
Include this class in your project by setting it as the custom GameInstance in the Unreal Project Settings.
## 🚧 TODO / Notes

  -  Expand support for avatars & crossplay filters

   - Integrate richer metadata queries for matchmaking

   - Add support for SDK-side party join requests when Epic updates support
