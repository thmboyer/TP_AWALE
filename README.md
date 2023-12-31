# TP_AWALE
**Basic client/server application for the Awale game**

## Compile & Run:
- To compile the client, go to the client directory and run make to compile the source code. To run the client, execute the prog.out file in the bin/ directory: **bin/prog.out <server_ipv4_address> <username>**
- To compile the server, go to the server directory and run make to compile the source code. To run the server, execute the prog.out file in the bin/ directory: **bin/prog.out**

## User Commands:
- **/users** -> To get the list of users connected on the server -> code: **/000**
- **/fight** <username> -> To send a challenge to another player, if they challenged you already, you get in a game with them -> code: **/001 <username(USERNAME_SIZE chars)>**
- **/games** -> To get the list of all the games going on -> code: **/002**
- **/plays** <pit_number> -> To make a play -> code: **/003**
- **/watch** <username> -> To watch the games of a player -> code: **/004 <username(USERNAME_SIZE chars)>** (a client can type /watch <own_username> to stop watching someone)
- **/chbio** <bio> -> To change bio -> code: **/005 <bio(BIO_SIZE chars)>**
- **/shbio** <username> -> To show the bio of a user -> **/006 <username(USERNAME_SIZE chars)>**
- **/addfr** <username> -> To send a friend request to a user -> **/007 <username(USERNAME_SIZE chars)>**
- **/frnds** -> To see your friendlist -> **/008**
- **/privt** -> Toggles private mode, only users in the FriendList can watch their games -> **/009**
- **/leave** -> To leave a game, the player leaving will lose. -> **/010**
- **/hgame** -> To get the list of all the previous games -> code : **/011**
- **/rgame** <game_id> -> To rewatch a previous game -> code : **/012**
