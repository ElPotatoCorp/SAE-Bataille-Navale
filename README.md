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

#### 🟦 Compilation avec Visual Studio 2022

> 🎯 **Solution toute prête :**  
> Le projet contient une solution Visual Studio (`.sln`) compatible avec Visual Studio 2022 (et ultérieur).  
> Il suffit d’ouvrir le fichier `SAE-Bataille-Navale.sln` avec Visual Studio, de sélectionner la configuration souhaitée (`Debug` ou `Release`) et de cliquer sur **"Générer"**.  
> Le projet est automatiquement configuré pour compiler avec le compilateur C++ de VS2022.
>
> - **Aucune ligne de commande requise**
> - Idéal pour les environnements Windows récents
> - Prend en charge le débogage graphique et la gestion des dépendances

#### Compilation manuelle :
```bash
git clone https://github.com/ElPotatoCorp/SAE-Bataille-Navale.git
cd SAE-Bataille-Navale
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32
```
> ⚠️ **Attention :**  
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

> 💡 **Astuce :** Vous pouvez également utiliser le script `build_win.ps1` fourni pour compiler automatiquement le projet sous Windows. Il suffit de lancer :
> ```
> build_win.ps1
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

Le lancement du jeu se fait désormais via une interface en ligne de commande plus flexible. Selon votre rôle (serveur ou joueur), vous pouvez démarrer le programme avec différents arguments.

### Usage général

```bash
main.exe [--server | -s] [--debug]
main.exe [--player | -p] <ip_address> [--debug]
main.exe [--player | -p] [--host | -h] [--debug]

# ou, sous Linux :
./main [--server | -s] [--debug]
./main [--player | -p] <ip_address> [--debug]
./main [--player | -p] [--host | -h] [--debug]
```

### Options

- `--server`, `-s` : Démarre en mode serveur (relai pour deux joueurs)
- `--player`, `-p <ip>` : Démarre en mode joueur et se connecte à un serveur à l’adresse `<ip>`
- `--player`, `-p --host`, `-h` : Démarre en mode joueur et héberge la partie (attend la connexion d’un autre joueur)
- `--debug` : Active l’affichage des informations de debug
- `--help`, `-h` : Affiche l’aide

### Exemples

- Démarrer le serveur de relais (à exécuter sur l’ordinateur qui fera office de serveur) :
  ```bash
  main.exe --server --debug
  # ou
  ./main -s
  ```

- Démarrer un joueur qui se connecte à un serveur :
  ```bash
  main.exe --player 192.168.1.42
  # ou
  ./main -p 192.168.1.42 --debug
  ```

- Démarrer un joueur qui héberge la partie (mode "host") :
  ```bash
  main.exe --player --host
  # ou
  ./main -p -h
  ```

> ℹ️ **Remarque :**
> - Les joueurs peuvent se connecter soit à un serveur "relai" central, soit se connecter entre eux si l'un d'eux utilise le mode `--host`.
> - Une fois la connexion établie, suivez les instructions à l’écran pour placer vos bateaux et commencer la partie !

---

## 📂 Structure du projet

- `battleship.h`, `battleship.c` — Logique du jeu, structures de données
- `win/` — Code réseau Windows (client_win.h/c, server_win.h/c)
- `linux/` — Code réseau Linux (client_linux.h/c, server_linux.h/c)
- `main.c` — Point d’entrée principal (orchestration des modes client/serveur)
- `README.md` — Ce document
