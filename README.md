# 🚢 SAE-Bataille-Navale

Un jeu de bataille navale en ligne sur réseau local (LAN), réalisé en C pour Windows et Linux, avec communication réseau via sockets.  
Affrontez vos amis dans un duel stratégique de navires — en mode local ou à distance sur votre réseau !

---

## 📋 Sommaire

- [Aperçu](#-aperçu)
- [Fonctionnalités](#-fonctionnalités)
- [Architecture](#-architecture)
- [Installation et Compilation](#-installation-et-compilation)
    - [Sous Windows](#sous-windows)
    - [Sous Linux](#sous-linux)
- [Lancement du jeu](#-lancement-du-jeu)
- [Structure du projet](#-structure-du-projet)

---

## 🖼️ Aperçu

> **SAE-Bataille-Navale** permet à deux joueurs de s’affronter sur une grille 10x10, avec gestion complète des flottes, tirs, et affichage console interactif.  
> La communication passe par TCP/IP sur le réseau local.

---

## ✨ Fonctionnalités

- 🎮 Bataille navale classique à 2 joueurs sur réseau local
- 🖥️ Support Windows **et** Linux (code et sockets adaptés à chaque plateforme)
- ⛴️ Placement manuel des navires avec validation intelligente
- 🚀 Affichage console dynamique et intuitif
- 📡 Communication réseau fiable (sockets TCP, détection d’erreurs)
- 🔄 Sérialisation du plateau pour transfert réseau
- 🐞 Mode debug pour le développement et le diagnostic

---

## 🏗️ Architecture

```
.
├── battleship.h/.c      # Logique du jeu et structures communes
├── win/                 # Code spécifique Windows (client & serveur)
├── linux/               # Code spécifique Linux (client & serveur)
├── main.c               # Point d'entrée principal
└── README.md            # Ce fichier !
```

- **battleship.h/c**: Toute la logique du jeu (placement, tirs, affichage…)
- **win/**: Fonctions réseau basées sur Winsock2 pour Windows
- **linux/**: Fonctions réseau basées sur sockets POSIX pour Linux

---

## ⚙️ Installation et Compilation

### Sous Windows

Prérequis :
- [MinGW-w64](https://www.mingw-w64.org/) ou autre compilateur C compatible
- [Git](https://git-scm.com/)

Compilation manuelle :
```bash
git clone https://github.com/ElPotatoCorp/SAE-Bataille-Navale.git
cd SAE-Bataille-Navale
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32
```
> ⚠️ **Attention :**  
> Si vous voyez une erreur similaire à :
> ```
> win/client_win.c: In function 'connect_to_server':
> win/client_win.c:35:9: warning: implicit declaration of function 'inet_pton' [-Wimplicit-function-declaration]
>      if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) != 1) {
>          ^~~~~~~~~
> C:\Users\XXX\AppData\Local\Temp\ccMLGspN.o:client_win.c:(.text+0x13c): undefined reference to `inet_pton'
> ```
> cela signifie que votre version de MinGW ne fournit pas la fonction `inet_pton`.  
> Pour corriger ce problème, il est recommandé de télécharger une version plus récente du compilateur via [MSYS2](https://www.msys2.org/), qui propose des versions de MinGW-w64 à jour et compatibles.

> 💡 **Astuce :** Vous pouvez également utiliser le script `build_win.bat` fourni pour compiler automatiquement le projet sous Windows. Il suffit de lancer :
> ```
> build_win.bat
> ```

---

### Sous Linux

Prérequis :
- `gcc` (ou équivalent)
- `make` (optionnel)

Compilation manuelle :
```bash
git clone https://github.com/ElPotatoCorp/SAE-Bataille-Navale.git
cd SAE-Bataille-Navale
gcc -o main main.c battleship.c linux/server_linux.c linux/client_linux.c
```
> 💡 **Astuce :** Vous pouvez aussi utiliser le script `build_linux.sh` pour compiler plus facilement :
> ```bash
> ./build_linux.sh
> ```

> **Astuce avancée :** Un `Makefile` peut aussi être ajouté pour faciliter la compilation multi-plateforme !

---

## 🚦 Lancement du jeu

Pour lancer le jeu, chaque joueur doit exécuter la commande suivante :

```bash
main.exe [ip_address_opposant] [player]
# ou, sous Linux :
./main [ip_address_opposant] [player]
```

- `[ip_address_opposant]` : l’adresse IP de l’autre joueur (votre adversaire) sur le réseau local.
- `[player]` : le numéro du joueur (`1` ou `2`).  
  → Le joueur 1 et le joueur 2 doivent choisir un numéro différent, et chacun doit renseigner l’adresse IP de l’autre.

**Exemples :**
- Sur l’ordinateur du joueur 1 (l’adresse IP du joueur 2 est `192.168.1.42`) :
  ```bash
  main.exe 192.168.1.42 1
  ```
- Sur l’ordinateur du joueur 2 (l’adresse IP du joueur 1 est `192.168.1.17`) :
  ```bash
  ./main 192.168.1.17 2
  ```

> ℹ️ **Remarque :**  
> Il n’est pas nécessaire de lancer les deux jeux exactement au même moment.  
> Chaque instance attendra patiemment la connexion de son adversaire.  
> Une fois la connexion établie, suivez les instructions à l’écran pour placer vos bateaux et commencer la partie !

---

## 📂 Structure du projet

- `battleship.h`, `battleship.c` — Logique du jeu, structures de données
- `win/` — Code réseau Windows (client_win.h/c, server_win.h/c)
- `linux/` — Code réseau Linux (client_linux.h/c, server_linux.h/c)
- `main.c` — Point d’entrée principal (orchestration des modes client/serveur)
- `README.md` — Ce document
