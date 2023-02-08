# Messages sent by client (with space-separated arguments):

* TYPE: logIn
  * ARGUMENTS: USERNAME PASSWORD

* TYPE: logOut
  * ARGUMENTS:
  
* TYPE: queueForGame
  * ARGUMENTS:
  
* TYPE: leaveQueue
  * ARGUMENTS:
  
* TYPE: sendWord
  * ARGUMENTS: WORD

# Messages sent by server (with space-separated arguments):

* TYPE: connectionCheck (unused)
  * ARGUMENTS:
 
* TYPE: logInResult
  * ARGUMENTS: success USERNAME
  * ARGUMENTS: FAILURES 
    
* TYPE: logOutResult
  * ARGUMENTS: logOutResultType

* TYPE: addedToQueue
  * ARGUMENTS: genericResultType
 
* TYPE: removedFromQueue
  * ARGUMENTS: genericResultType
 
* TYPE: gameReady
  * ARGUMENTS: OTHER_PLAYER_USERNAME FIRST_CHARSET
  
* TYPE: gameOver
  * ARGUMENTS: gameOverParams

* TYPE: playerAdvancedRound
  * ARGUMENTS: NEXT_CHARSET

* TYPE: enemyAdvancedRound
  * ARGUMENTS: 

* TYPE: answerRejected
  * ARGUMENTS:
             
    
