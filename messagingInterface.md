#Messages sent by client (with space-separated arguments):

TYPE:logIn
ARGUMENTS: <USERNAME> <PASSWORD>

TYPE:logOut
ARGUMENTS:
  
TYPE:queueForGame
ARGUMENTS:
  
TYPE:leaveQueue
ARGUMENTS:
  
TYPE:sendWord
ARGUMENTS: <WORD>

#Messages sent by server (with space-separated arguments):

(unused)
TYPE:connectionCheck
ARGUMENTS:
 
TYPE:logInResult
ARGUMENTS: success <USERNAME>
           <FAILURES> 
    
TYPE:logOutResult
ARGUMENTS: <logOutResultType>
             
    
