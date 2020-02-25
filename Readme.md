System reuqirements: 32 bit ubuntu VM

About the setup:-

- There is a server who is listening to multiple clients at a same time.
 - The clients make a connection to port 4444(any open port) on which server is listening.
 - The server on receiving the request redirects to a different port and assigns a child thread to the remote client.
 - So every connection request a new child is forked.

What is clone? Why use it?

 - When we want to create multiple processes which belong to a single parent we use clone.
 - Clone gives all freedom to the programmer to manage the resources of parent and child.
 - Structure of clone: int clone(int (*fn)(void *), void *stack, int flags, void *arg, ... /* pid_t *parent_tid, void *tls, pid_t *child_tid */ );

Are we using fork? NO!
 - Fork is similar to clone. In fact Fork internally calls clone with different parameters.
 - When we fork, the child is given the same resources as of the parent.
 - Fork uses Copy-on-write approach in which a child replicates the stack of the parent before writing anything to it
 Note: Both parent and child uses same virtual memory address but “Physical address space is different”. We need both the physical address space to be shared by child and parent for dangling pointer to work.

What is linker interpositioning?

 - Say you have a binary and cannot reverse? But still you want to add custom functionaility to that binary.
 - Compile time Linker interpositioning will help to add our own function to the exisiting executable object file.
 - This will allow the attacker to perform malicious operation by injecting his own code.
 - The new executable will have dangling pointer and format string vulnerability.

Applications: -
 - Session hijacking
 - Shell code execution on the remote server.
 - Evesdropping or tapping a tcp communications.

Functionality and how to exploit.
 - here i have assumed that the client was able to find the baseadress of the dead_Code which was triggered due to bad payload. 
 - In real the payload is sent over the network and run on the server which jumps the control flow to the deadcode but here we will have a placeholder instead.
 - Let us pass a string "dummy" which is a replacement to that payload.
 - We pass the payload as the argument and hence when the server receives the string "dummy" from the client it will put the thread on waiting state.
 - As soon as another sibling thread sends a data, due to dangling pointer vulnerability the first thread which was waiting gets the same data(as both share the same address space) and return the message received from the different client.

 Run tcpServer.c <payload> on a terminal
 Run tcpClient.c on a different terminal and keep sharing information.
 Connect another client to the server by running client script again on a seperate terminal. At present both the terminal are able to communicate and are able to get the exact same message from the server they sent.
 Now from one of the client terminal run dummy. This triggers the attackers payload and the thread waits.
 Immediately send a data to the server from yet different client. This will assign same resources to the new client and the client gets back the reply.
 The attacker client gets the reply same as the other client got. Hence message sniffed.
