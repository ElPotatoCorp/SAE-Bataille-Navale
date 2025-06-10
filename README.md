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

```bash
git clone https://github.com/ElPotatoCorp/SAE-Bataille-Navale.git
cd SAE-Bataille-Navale
gcc -o main.exe main.c battleship.c win/server_win.c win/client_win.c -lws2_32
```

### Sous Linux

Prérequis :
- `gcc` (ou équivalent)
- `make` (optionnel)

```bash
git clone https://github.com/ElPotatoCorp/SAE-Bataille-Navale.git
cd SAE-Bataille-Navale
gcc -o main main.c battleship.c linux/server_linux.c linux/client_linux.c
```

> **Astuce :** Un `Makefile` peut être ajouté pour faciliter la compilation multi-plateforme !

---

## 🚦 Lancement du jeu

1. **Lancer le serveur** (sur une machine du réseau) :
   - Windows : exécuter `main.exe` en mode serveur
   - Linux : exécuter `./main` en mode serveur

2. **Lancer le client** (sur une machine ou la même) :
   - Entrez l’IP du serveur lorsque demandé.
   - Suivez les instructions à l’écran pour placer vos bateaux et jouer !

---

## 📂 Structure du projet

- `battleship.h`, `battleship.c` — Logique du jeu, structures de données
- `win/` — Code réseau Windows (client_win.h/c, server_win.h/c)
- `linux/` — Code réseau Linux (client_linux.h/c, server_linux.h/c)
- `main.c` — Point d’entrée principal (orchestration des modes client/serveur)
- `README.md` — Ce document

---

## 📜 Licence

Projet sous licence MIT.  
Voir [LICENSE](LICENSE) pour les détails.

---

<p align="center">
  <img src="https://upload.wikimedia.org/wikipedia/commons/3/32/Battleship_game_board.svg" alt="Battleship" width="300"/>
</p>
