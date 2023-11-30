# TP_AWALE
**Basic client/server application for the Awale game**

## User Commands:
- **/users** -> To get the list of users connected on the server -> code: **/000**
- **/fight** <pseudo> -> To send a challenge to another player, if he challenged you already, you get in a game with him -> code: **/001 <pseudo(10 chars)>**
- **/games** -> To get the list of all the games going on -> code: **/002**
- **/plays <pit_number>** -> To make a play -> code: **/003**
- **/watch <pseudo>** -> To watch the game of a player -> code: **/004 <pseudo(10 chars)>** (a client can type /watch <own_pseudo> to stop watching a game)

- [ ] bio [ ]
- [ ] Mode privé avec friendlist
- [ ] add (me) next to self
- [ ] Concede
- [ ] Persistance
- [ ] Pw or only one per username
- [ ] Timeout on requests
- [ ] mute
- [ ] Jouer plusieurs parties à la fois (problématique avec les observers)
