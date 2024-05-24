# cps

### Utilisation

- make
- ./serveur <port_proxy>
- ./proxy <port_local> <port_serveur>
- ./client <port_serveur>
- Mettre les messages dans le client

### Exemple d'utilisation

- ./serveur 9090
- ./proxy 8080 9090
- ./client 8080
- Un autre client
- ./client 8080
