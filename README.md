## Run it

Client: 
```bash
cd src/client
gcc -o client client.c ./utils/network.c ./utils/file.c -I./utils -lnsl
./client localhost
```

Server:
```bash
cd src/server
gcc -o server server.c ./utils/network.c ./utils/thread.c -I./utils -lnsl
./server
```
