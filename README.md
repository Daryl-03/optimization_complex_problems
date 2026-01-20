# TP1 - Optimisation Multi-Objectif

### Visualisation
- **Gnuplot** : nécessaire pour l'affichage des graphiques
  - Windows : [Télécharger Gnuplot](http://www.gnuplot.info/download.html)
  - Linux : `sudo apt-get install gnuplot`
  - Mac : `brew install gnuplot`

## Installation

### 1. Cloner ou télécharger le projet

```bash
cd chemin/vers/le/projet
```

### 2. Configurer le chemin des instances

Dans le fichier `utils.c`, modifier le chemin de base vers les instances si nécessaire :

```c
const char *basePath = "C:.....\\instances\\";
```

Remplacer par votre chemin absolu vers le dossier `instances/`.


### Changer l'instance traitée

Pour les questions qui utilisent une seule instance, modifier l'appel à `getInstanceByName()` :

```c
Instance *instance = getInstanceByName("20_10_01.txt");
```

Instances disponibles :
- `7_5_01.txt` - 7 jobs, 5 machines (petite)
- `20_10_01.txt` - 20 jobs, 10 machines (moyenne)
- `20_20_01.txt` - 20 jobs, 20 machines
- `30_10_01.txt` - 30 jobs, 10 machines
- `30_20_01.txt` - 30 jobs, 20 machines
- `50_10_01.txt` - 50 jobs, 10 machines (grande)
- `50_20_01.txt` - 50 jobs, 20 machines

## Structure du projet

```
tp1/
├── main.c              # Point d'entrée, sélection des questions
├── lib.h / lib.c       # Structures de données et fonctions de base
├── multivar.h / .c     # Algorithmes multi-objectifs (filtrage, scalaire, Pareto)
├── utils.h / .c        # Utilitaires (lecture instances, export gnuplot)
├── analyse.h / .c      # Fonctions d'analyse pour chaque question
├── CMakeLists.txt      # Configuration CMake
├── instances/          # Fichiers d'instances du problème
└── cmake-build-debug/  # Dossier de compilation
```


