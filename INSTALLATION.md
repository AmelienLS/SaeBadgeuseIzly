# Installation Rapide - SAE Badgeuse RFID

Guide d'installation complet pour mettre en place le projet après clonage depuis GitHub.

## Prérequis

- **macOS** (ou Linux)
- **Python 3.7+** installé
- **Git** installé
- **Arduino UNO R4 WiFi** (ou Minima pour tests sans WiFi)

### Vérifier Python

```bash
python3 --version
# Devrait afficher: Python 3.x.x
```

Si Python n'est pas installé :
```bash
# Sur macOS avec Homebrew
brew install python3
```

## Installation en 3 étapes

### Étape 1 : Cloner le projet

```bash
# Cloner depuis GitHub
git clone <URL_DU_REPO>
cd SaeBadgeuseIzly

# Ou si vous avez déjà téléchargé
cd SaeBadgeuseIzly
```

### Étape 2 : Configuration automatique

```bash
# Rendre les scripts exécutables
chmod +x *.sh

# Lancer le setup automatique
./setup_env.sh
```

Ce script va automatiquement :
- ✅ Créer un environnement virtuel Python isolé
- ✅ Installer PlatformIO dans cet environnement
- ✅ Configurer tous les outils nécessaires
- ✅ Créer les scripts d'activation rapide

**Durée** : ~2-3 minutes

### Étape 3 : Tester la compilation

```bash
# Compiler le projet
./build.sh
```

Si la compilation réussit, vous verrez :
```
========================= [SUCCESS] Took X.XX seconds =========================
RAM:   [==        ]  18.2% (used 5960 bytes from 32768 bytes)
Flash: [===       ]  26.8% (used 70300 bytes from 262144 bytes)
```

## Configuration du projet

### 1. Configuration WiFi et Serveur

Éditez [src/main.cpp](src/main.cpp) et modifiez :

```cpp
// Ligne 9-10 : WiFi
const char WIFI_SSID[] = "VotreSsidWiFi";
const char WIFI_PASSWORD[] = "VotreMotDePasse";

// Ligne 13-15 : Serveur
const char SERVER_HOST[] = "votre-serveur.com";
const int SERVER_PORT = 80;  // ou 443 pour HTTPS
const char SERVER_PATH[] = "/export_etudiants.php";
```

### 2. Configuration de la base de données

```bash
# Sur votre serveur MySQL
mysql -u root -p SRC_DB < etudiants_rfid.sql

# Ajouter vos étudiants
mysql -u root -p SRC_DB
```

```sql
INSERT INTO etudiants (uid_rfid, nom, prenom, codeGroupe, email) VALUES
('04A1B2C3', 'DUPONT', 'Jean', 15824990, 'jean.dupont@example.com');
```

### 3. Configuration du script PHP

Copiez `export_etudiants.php` sur votre serveur et modifiez :

```php
define('DB_HOST', 'localhost');
define('DB_NAME', 'DST_DB');
define('DB_USER', 'votre_utilisateur');
define('DB_PASS', 'votre_mot_de_passe');
```

## Utilisation quotidienne

### Compiler et téléverser

```bash
# Compiler uniquement
./build.sh

# Compiler et téléverser sur l'Arduino
./upload.sh

# Moniteur série (pour voir les logs)
./monitor.sh
```

### Commandes PlatformIO directes

```bash
# Activer l'environnement virtuel
source .venv/bin/activate

# Utiliser pio normalement
pio run                              # Compiler
pio run -t upload                    # Téléverser
pio device monitor -b 115200         # Moniteur série
pio device list                      # Lister les ports

# Quitter l'environnement
deactivate
```

## Structure du projet

```
SaeBadgeuseIzly/
├── .venv/                      # Environnement virtuel (ignoré par git)
├── .gitignore                  # Ignore .venv, .pio, etc.
├── requirements.txt            # Dépendances Python
├── setup_env.sh                # Setup automatique
├── build.sh                    # Compilation rapide
├── upload.sh                   # Téléversement rapide
├── monitor.sh                  # Moniteur série
├── src/
│   └── main.cpp               # Code Arduino principal
├── platformio.ini             # Configuration PlatformIO
├── etudiants_rfid.sql         # Schéma de la table étudiants
├── export_etudiants.php       # API d'export CSV
├── README.md                  # Documentation principale
├── INSTALLATION.md            # Ce fichier
├── VERIFICATION.md            # Guide de vérification
└── ENVIRONNEMENT_VIRTUEL.md   # Doc sur l'env virtuel
```

## Fichiers ignorés par Git

Le `.gitignore` est configuré pour ignorer :
- `.venv/` - Environnement virtuel Python
- `.pio/` - Build artifacts de PlatformIO
- `*.log` - Fichiers de logs
- `.DS_Store` - Fichiers macOS
- `credentials.h`, `secrets.h` - Fichiers de credentials (sécurité)

## Installation manuelle (alternative)

Si `setup_env.sh` ne fonctionne pas :

```bash
# 1. Créer l'environnement virtuel
python3 -m venv .venv

# 2. Activer l'environnement
source .venv/bin/activate

# 3. Installer les dépendances
pip install -r requirements.txt

# 4. Tester
pio --version
```

## Partage avec l'équipe

### Pour pousser sur GitHub

```bash
# Ajouter tous les fichiers (sauf ceux dans .gitignore)
git add .

# Commit
git commit -m "Initial commit - Système de badgeuse RFID"

# Push
git push origin main
```

Le `.gitignore` garantit que `.venv/` et `.pio/` ne seront **jamais** poussés sur GitHub.

### Pour un nouveau collaborateur

```bash
# 1. Cloner le repo
git clone <URL>
cd SaeBadgeuseIzly

# 2. Setup automatique
chmod +x *.sh
./setup_env.sh

# 3. C'est prêt !
./build.sh
```

Chaque développeur aura son propre environnement virtuel local, isolé et fonctionnel.

## Dépannage

### Problème : "python3: command not found"

```bash
# Installer Python 3
brew install python3
```

### Problème : "Permission denied" sur les scripts

```bash
# Rendre les scripts exécutables
chmod +x *.sh
```

### Problème : L'environnement ne s'active pas

```bash
# Supprimer et recréer
rm -rf .venv
./setup_env.sh
```

### Problème : La compilation échoue

```bash
# Nettoyer et recompiler
.venv/bin/pio run -t clean
./build.sh
```

### Problème : "Module 'platformio' not found"

```bash
# Réinstaller dans l'environnement
source .venv/bin/activate
pip install --upgrade platformio
```

## Commandes utiles

```bash
# Vérifier l'état de l'environnement
source .venv/bin/activate
which python    # Devrait pointer vers .venv/bin/python
which pio       # Devrait pointer vers .venv/bin/pio
pio --version   # Vérifier la version

# Mettre à jour PlatformIO
pip install --upgrade platformio

# Lister les dépendances installées
pip list

# Nettoyer complètement le projet
rm -rf .venv .pio
./setup_env.sh
```

## Avantages de cette approche

✅ **Isolation** : Chaque développeur a son environnement isolé
✅ **Reproductible** : Le setup est identique pour tous
✅ **Pas de pollution** : Rien n'est installé globalement
✅ **Versionné** : `requirements.txt` garantit les bonnes versions
✅ **Rapide** : Setup automatique en 3 minutes
✅ **Propre** : `.gitignore` empêche de commit les fichiers inutiles

## Support

Pour des problèmes d'installation :
1. Consultez [VERIFICATION.md](VERIFICATION.md)
2. Consultez [ENVIRONNEMENT_VIRTUEL.md](ENVIRONNEMENT_VIRTUEL.md)
3. Vérifiez que Python 3.7+ est installé
4. Essayez l'installation manuelle

## Checklist de validation

Après installation, vérifiez que :

- [ ] `.venv/` existe et contient Python
- [ ] `./build.sh` compile sans erreur
- [ ] `pio --version` affiche la version (après activation)
- [ ] Le `.gitignore` empêche de commit `.venv/`
- [ ] Les credentials WiFi/Server sont configurés dans `src/main.cpp`

Si tous les points sont ✅, vous êtes prêt à développer ! 🚀
