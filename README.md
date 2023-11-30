# TP_AWALE
**Basic client/server application for the Awale game**

All the options such as buffer sizes are written in options.h

## User Commands:
- **/users** -> To get the list of users connected on the server -> code: **/000**
- **/fight** <username> -> To send a challenge to another player, if he challenged you already, you get in a game with him -> code: **/001 <username(USERNAME_SIZE chars)>**
- **/games** -> To get the list of all the games going on -> code: **/002**
- **/plays** <pit_number> -> To make a play -> code: **/003**
- **/watch** <username> -> To watch the game of a player -> code: **/004 <username(USERNAME_SIZE chars)>** (a client can type /watch <own_username> to stop watching a game)
- **/chbio** <bio> -> To change bio -> code: **/005 <bio(BIO_SIZE chars)>**
- **/shbio** <username> -> To show the bio of a user -> **/006 <username(USERNAME_SIZE chars)>**

- [ ] bio
- [ ] Mode privé avec friendlist
- [ ] add (me) next to self
- [ ] Concede
- [ ] Persistance
- [ ] Pw or only one per username
- [ ] Timeout on requests
- [ ] mute
- [ ] Jouer plusieurs parties à la fois (problématique avec les observers)
