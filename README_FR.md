# 🖥️ UIMonitor - Moniteur de Ressources Système

[![English](https://img.shields.io/badge/lang-English-red.svg)](README.md)

Un moniteur de ressources système léger et performant en interface terminal (TUI) pour Windows, écrit en C++.
Inspiré par des outils comme `htop` et `btop`, UIMonitor fournit des informations en temps réel sur les performances de votre système directement depuis votre console.

## 📋 Fonctionnalités

- ✅ **Surveillance CPU Temps Réel** - Pourcentage d'utilisation globale du processeur.
- ✅ **Suivi de la Mémoire** - Consommation de RAM (Utilisée/Totale) avec jauge visuelle.
- ✅ **Gestionnaire de Processus** - Liste en direct des processus triés par usage CPU.
- ✅ **Haute Performance** - Faible surcharge grâce aux API natives Windows.
- ✅ **TUI Moderne** - Interface esthétique propulsée par FTXUI.

## 🚀 Installation / Compilation

### Prérequis
- **Windows 10/11**
- **Visual Studio 2022** (avec la charge de travail "Développement Desktop C++")
- **CMake 3.10+**
- **Git**

### Compilation

1.  **Cloner le dépôt** :
    ```powershell
    git clone https://github.com/votreutilisateur/UIMonitor.git
    cd UIMonitor
    ```

2.  **Initialiser vcpkg** :
    ```powershell
    ./vcpkg/bootstrap-vcpkg.bat
    ```

3.  **Configurer le projet** :
    ```powershell
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
    ```

4.  **Compiler** :
    ```powershell
    cmake --build build --config Release
    ```

5.  **Lancer** :
    ```powershell
    ./build/Release/UIMonitor.exe
    ```

## 📦 Structure du Projet

```
UIMonitor/
├── include/
│   ├── CpuMonitor.hpp
│   ├── MemoryMonitor.hpp
│   └── ProcessMonitor.hpp
├── src/
│   ├── main.cpp
│   ├── CpuMonitor.cpp
│   ├── MemoryMonitor.cpp
│   └── ProcessMonitor.cpp
├── vcpkg/
├── CMakeLists.txt
├── vcpkg.json
└── LICENSE
```

## 🎨 Fonctionnalités Détaillées

### 1️⃣ Surveillance CPU
Calcule l'utilisation globale du CPU via l'API native Windows `GetSystemTimes`.
- **Précision** : Calcule le delta entre les temps kernel/user et les temps d'inactivité.
- **Efficacité** : S'exécute sur un thread séparé pour ne pas bloquer l'interface.

### 2️⃣ Surveillance Mémoire
Récupère les statistiques de la mémoire système via `GlobalMemoryStatusEx`.
- **Visuel** : Affiche une jauge montrant la mémoire physique utilisée vs totale.
- **Stats** : Affiche le pourcentage exact et l'utilisation en Go.

### 3️⃣ Gestionnaire de Processus
Maintient une liste en direct des processus actifs via `CreateToolhelp32Snapshot`.
- **Tri** : Trie automatiquement le top 20 des processus par utilisation CPU.
- **Optimisation** : Utilise `std::unordered_map` pour une recherche et un calcul de delta en O(1).
- **Détails** : Affiche le PID, le Nom du Processus, le % CPU et le Working Set Mémoire.

### 4️⃣ Interface Terminal (FTXUI)
Construit avec **FTXUI** (Functional Terminal User Interface).
- **Double Buffering** : Rendu sans scintillement.
- **Responsive** : S'adapte à la taille du terminal.
- **Interactive** : Mise en page propre et moderne.

## 🙏 Remerciements

- **[FTXUI](https://github.com/ArthurSonzogni/FTXUI)** par Arthur Sonzogni
- **[vcpkg](https://github.com/microsoft/vcpkg)** par Microsoft
- **Windows API** documentation
