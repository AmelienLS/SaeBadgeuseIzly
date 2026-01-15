# Guide de l'Environnement Virtuel Python

## Pourquoi un environnement virtuel ?

L'environnement virtuel Python permet de :
- ✅ **Isoler** les dépendances du projet (PlatformIO) de votre système
- ✅ **Éviter** les conflits avec d'autres projets Python
- ✅ **Résoudre** les problèmes de PATH dans votre `.zprofile`
- ✅ **Garantir** une version spécifique de PlatformIO
- ✅ **Simplifier** le partage du projet (via `requirements.txt`)

## Installation initiale

### 1. Configuration de l'environnement

```bash
# Rendre le script exécutable (une seule fois)
chmod +x setup_env.sh

# Lancer la configuration
./setup_env.sh
```

Ce script va :
1. Vérifier que Python 3 est installé
2. Créer un environnement virtuel dans `.venv/`
3. Installer PlatformIO dans cet environnement isolé
4. Créer un script d'activation rapide

### 2. Rendre les autres scripts exécutables

```bash
chmod +x build.sh upload.sh monitor.sh activate.sh
```

## Utilisation quotidienne

### Méthode 1 : Scripts automatisés (recommandé)

Les scripts gèrent automatiquement l'activation de l'environnement :

```bash
# Compiler
./build.sh

# Téléverser sur l'Arduino
./upload.sh

# Moniteur série
./monitor.sh
```

### Méthode 2 : Activation manuelle

Si vous préférez garder le contrôle :

```bash
# Activer l'environnement
source .venv/bin/activate

# Ou utilisez le script rapide
./activate.sh

# Maintenant vous pouvez utiliser pio normalement
pio run -e uno_r4_wifi
pio run -e uno_r4_wifi --target upload
pio device monitor -b 115200

# Quand vous avez fini
deactivate
```

## Structure du projet

```
SaeBadgeuseIzly/
├── .venv/                    # Environnement virtuel (ignoré par git)
│   ├── bin/
│   │   ├── python           # Python isolé
│   │   ├── pip              # pip isolé
│   │   └── pio              # PlatformIO isolé
│   └── lib/                 # Bibliothèques Python isolées
├── setup_env.sh             # Configuration initiale
├── activate.sh              # Activation rapide
├── build.sh                 # Compilation
├── upload.sh                # Téléversement
├── monitor.sh               # Moniteur série
├── src/
│   └── main.cpp            # Code Arduino
├── platformio.ini          # Configuration PlatformIO
└── .gitignore              # .venv/ est ignoré
```

## Commandes disponibles

### Configuration

```bash
./setup_env.sh              # Installation initiale (une fois)
./activate.sh               # Activer l'environnement rapidement
```

### Développement

```bash
./build.sh                  # Compiler le projet
./upload.sh                 # Compiler + Téléverser
./monitor.sh                # Ouvrir le moniteur série
```

### PlatformIO (après activation)

```bash
source .venv/bin/activate   # Activer d'abord

pio run                     # Compiler toutes les configs
pio run -e uno_r4_wifi      # Compiler pour UNO R4 WiFi
pio run -t upload           # Téléverser
pio run -t clean            # Nettoyer le build
pio device list             # Lister les ports série
pio device monitor          # Moniteur série
pio lib search <nom>        # Chercher une bibliothèque
pio lib install <nom>       # Installer une bibliothèque

deactivate                  # Désactiver quand terminé
```

## Avantages de cette approche

### 1. Isolation complète

```
Système macOS
├── Python système (/usr/bin/python3)
│   └── Bibliothèques système
│
└── Projet SaeBadgeuseIzly
    ├── Python isolé (.venv/bin/python)
    └── PlatformIO isolé (.venv/bin/pio)
        ├── Version spécifique
        └── Pas de conflit avec le système
```

### 2. Reproductibilité

Créez un `requirements.txt` pour partager la configuration :

```bash
# Générer le fichier des dépendances
source .venv/bin/activate
pip freeze > requirements.txt
```

Sur une autre machine :

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
```

### 3. Plusieurs versions

Vous pouvez avoir plusieurs projets avec différentes versions de PlatformIO :

```
~/ProjetA/.venv/  → PlatformIO 6.1.0
~/ProjetB/.venv/  → PlatformIO 6.2.0
~/ProjetC/.venv/  → PlatformIO 6.3.0
```

## Résolution de problèmes

### Problème : "command not found: python3"

```bash
# Installer Python 3 via Homebrew
brew install python3
```

### Problème : "Permission denied" sur les scripts

```bash
# Rendre les scripts exécutables
chmod +x *.sh
```

### Problème : L'environnement ne s'active pas

```bash
# Vérifier que le dossier existe
ls -la .venv/

# Si absent, recréer
rm -rf .venv
./setup_env.sh
```

### Problème : PlatformIO ne compile pas

```bash
# Vérifier que l'environnement est activé
which pio
# Devrait afficher: /chemin/vers/projet/.venv/bin/pio

# Si non activé
source .venv/bin/activate

# Nettoyer et recompiler
pio run -t clean
pio run -e uno_r4_wifi
```

### Problème : "Module not found" dans les scripts

```bash
# Réinstaller PlatformIO
source .venv/bin/activate
pip uninstall platformio
pip install platformio
```

## Comparaison avec l'installation système

| Aspect | Système global | Environnement virtuel |
|--------|---------------|----------------------|
| Installation | `pip install platformio` | `./setup_env.sh` |
| Activation | Automatique | `source .venv/bin/activate` |
| Conflits | Possibles | ❌ Aucun |
| Isolation | ❌ Non | ✅ Oui |
| Version | Unique | Multiple possible |
| Partage projet | Difficile | Facile (requirements.txt) |
| Nettoyage | Compliqué | `rm -rf .venv` |

## Workflow recommandé

### Première fois

```bash
# 1. Cloner le projet
git clone <url>
cd SaeBadgeuseIzly

# 2. Configurer l'environnement
chmod +x *.sh
./setup_env.sh

# 3. Compiler pour tester
./build.sh
```

### Tous les jours

```bash
# Ouvrir le terminal dans le projet
cd SaeBadgeuseIzly

# Compiler
./build.sh

# Téléverser
./upload.sh

# Moniteur
./monitor.sh
```

### Pour développer longtemps

```bash
# Activer une fois
source .venv/bin/activate

# Travailler normalement
pio run
# éditer le code
pio run
# tester
pio run -t upload

# À la fin
deactivate
```

## Intégration avec les IDE

### VSCode

Ajoutez dans `.vscode/settings.json` :

```json
{
  "python.defaultInterpreterPath": "${workspaceFolder}/.venv/bin/python",
  "python.terminal.activateEnvironment": true,
  "platformio.forceUploadAndMonitor": true
}
```

VSCode activera automatiquement l'environnement dans ses terminaux.

### Terminal intégré

Les scripts `build.sh`, `upload.sh`, etc. fonctionnent dans n'importe quel terminal car ils activent automatiquement l'environnement.

## Désinstallation

Si vous voulez tout supprimer :

```bash
# Supprimer l'environnement virtuel
rm -rf .venv/

# Supprimer les builds PlatformIO
rm -rf .pio/

# Optionnel : supprimer le cache PlatformIO global
rm -rf ~/.platformio/
```

Puis recommencez avec `./setup_env.sh`.

## Fichiers créés

- `setup_env.sh` : Configuration initiale de l'environnement
- `activate.sh` : Activation rapide (créé par setup_env.sh)
- `build.sh` : Compilation automatisée
- `upload.sh` : Téléversement automatisé
- `monitor.sh` : Moniteur série automatisé
- `.gitignore` : Mis à jour pour ignorer `.venv/`

## Avantages spécifiques à ce projet

1. **Évite les problèmes de PATH** dans votre `.zprofile`
2. **Isole PlatformIO** des autres outils Python
3. **Scripts simples** : `./build.sh` au lieu de commandes complexes
4. **Portable** : fonctionne sur n'importe quel Mac/Linux
5. **Partageable** : les autres membres de l'équipe peuvent setup en 1 commande

## Résumé des commandes essentielles

```bash
# Configuration (une fois)
./setup_env.sh

# Compilation
./build.sh

# Upload
./upload.sh

# Moniteur série
./monitor.sh

# Activation manuelle (optionnel)
source .venv/bin/activate
# ... commandes pio ...
deactivate
```

C'est tout ! 🎉

Votre environnement est maintenant complètement isolé et les problèmes de PATH sont résolus.
