# Système de Badgeuse RFID avec Arduino UNO R4 WiFi

Système de badgeage intelligent pour étudiants utilisant des cartes RFID avec synchronisation en temps réel vers une base de données MySQL via WiFi.

## Table des matières

- [Vue d'ensemble](#vue-densemble)
- [Caractéristiques](#caractéristiques)
- [Structure du projet](#structure-du-projet)
- [Prérequis](#prérequis)
- [Installation rapide](#installation-rapide)
- [Documentation détaillée](#documentation-détaillée)
- [Utilisation](#utilisation)
- [Configuration](#configuration)
- [Maintenance](#maintenance)
- [Dépannage](#dépannage)
- [Contribution](#contribution)
- [Licence](#licence)

## Vue d'ensemble

Ce projet implémente un système complet de badgeage RFID pour le suivi de présence des étudiants. Le système utilise un Arduino UNO R4 WiFi connecté à un lecteur RFID RC522 et une carte SD pour le stockage local des données. Les données sont synchronisées avec une base de données MySQL via une API PHP.

### Architecture du système

```
┌─────────────────────────┐
│   Base de données       │
│   MySQL (SRC_DB)        │
└───────────┬─────────────┘
            │
            ↓
┌─────────────────────────┐
│   Script PHP            │
│   export_etudiants.php  │
└───────────┬─────────────┘
            │ HTTP/WiFi
            ↓
┌─────────────────────────┐
│   Arduino UNO R4 WiFi   │
│   ├─ Lecteur RFID       │
│   │   (RC522)           │
│   └─ Carte SD           │
│      (stockage local)   │
└─────────────────────────┘
            ↑
            │ RFID
    ┌───────┴───────┐
    │ Carte étudiant│
    └───────────────┘
```

## Caractéristiques

- **Lecture RFID** : Identification instantanée des étudiants via cartes RFID
- **Stockage local** : Sauvegarde sur carte SD des badgeages (format CSV)
- **Synchronisation WiFi** : Téléchargement automatique de la base de données étudiants
- **Mode hors ligne** : Fonctionne même sans connexion réseau
- **Anti-rebond** : Évite les doubles lectures (cooldown de 1.5s)
- **Mise à jour automatique** : Base de données actualisée toutes les heures
- **Logs détaillés** : Monitoring via port série (115200 baud)
- **Format CSV** : Export compatible Excel/LibreOffice

## Structure du projet

```
SaeBadgeuseIzly/
├── src/                          # Code source Arduino
│   └── main.cpp                  # Programme principal (474 lignes)
│
├── database/                     # Fichiers SQL
│   ├── etudiants_rfid.sql       # Structure table étudiants
│   ├── CopybaseSimple.sql       # Script de synchronisation DB
│   ├── CopybaseSimple_etudiants_addon.sql
│   ├── Base_Seances.sql         # Base des séances
│   └── complete_SRC_DB.sql      # Structure complète SRC_DB
│
├── server/                       # Code serveur
│   └── export_etudiants.php     # API d'export CSV (62 lignes)
│
├── scripts/                      # Scripts d'automatisation
│   ├── setup_env.sh             # Configuration environnement
│   ├── build.sh                 # Compilation du projet
│   ├── upload.sh                # Téléversement Arduino
│   ├── monitor.sh               # Moniteur série
│   ├── check.sh                 # Vérification installation
│   ├── clean.sh                 # Nettoyage projet
│   └── fix_path.py              # Correction PATH macOS
│
├── docs/                         # Documentation
│   ├── INSTALLATION.md          # Guide installation détaillé
│   ├── QUICKSTART.md            # Démarrage rapide (5 min)
│   ├── VERIFICATION.md          # Tests et dépannage
│   ├── ENVIRONNEMENT_VIRTUEL.md # Doc environnement Python
│   ├── CONTRIBUTING.md          # Guide de contribution
│   ├── SUMMARY.md               # Résumé complet
│   ├── PROJECT_STATUS.md        # État du projet
│   └── FILES_GUIDE.md           # Guide des fichiers
│
├── platformio.ini                # Configuration PlatformIO
├── requirements.txt              # Dépendances Python
├── .gitignore                    # Fichiers à ignorer
└── README.md                     # Ce fichier
```

## Prérequis

### Matériel requis

- **Arduino UNO R4 WiFi** (ou UNO R4 Minima pour version sans WiFi)
- **Lecteur RFID RC522** (13.56 MHz)
- **Module carte SD** (compatible SPI)
- **Carte SD** (formatée en FAT32, min 512 Mo)
- **Cartes RFID** pour les étudiants (MIFARE Classic 1K ou compatible)
- **Câbles de connexion** (jumper wires)
- **Câble USB** pour la programmation

### Logiciels requis

- **Python 3.7+** (pour PlatformIO)
- **Git** (pour cloner le projet)
- **MySQL/MariaDB** (pour la base de données)
- **Serveur web** (Apache/Nginx avec PHP 7.4+)

### Dépendances Arduino (installées automatiquement)

- `miguelbalboa/MFRC522 @ ^1.4.11` - Bibliothèque lecteur RFID
- `arduino-libraries/SD @ ^1.3.0` - Bibliothèque carte SD
- `arduino-libraries/ArduinoHttpClient @ ^0.5.0` - Client HTTP
- `WiFiS3` (intégré) - Bibliothèque WiFi pour R4

## Installation rapide

### Étape 1 : Cloner le projet

```bash
git clone https://github.com/votre-repo/SaeBadgeuseIzly.git
cd SaeBadgeuseIzly
```

### Étape 2 : Configurer l'environnement

```bash
# Rendre les scripts exécutables
chmod +x scripts/*.sh

# Installer l'environnement virtuel et PlatformIO
./scripts/setup_env.sh

# Vérifier l'installation
./scripts/check.sh
```

### Étape 3 : Câblage du matériel

#### Lecteur RFID RC522

| RC522  | Arduino R4 WiFi |
|--------|-----------------|
| SDA    | Pin 7           |
| SCK    | SCK (SPI)       |
| MOSI   | MOSI (SPI)      |
| MISO   | MISO (SPI)      |
| IRQ    | Non connecté    |
| GND    | GND             |
| RST    | Pin 9           |
| 3.3V   | 3.3V            |

#### Module carte SD

| SD Card | Arduino R4 WiFi |
|---------|-----------------|
| CS      | Pin 10          |
| SCK     | SCK (SPI)       |
| MOSI    | MOSI (SPI)      |
| MISO    | MISO (SPI)      |
| VCC     | 5V              |
| GND     | GND             |
| PWR     | Pin 8           |

### Étape 4 : Configuration de la base de données

```bash
# Créer la table étudiants
mysql -u root -p SRC_DB < database/etudiants_rfid.sql

# Ajouter vos étudiants (modifiez le fichier SQL avant)
# Exemple :
# INSERT INTO etudiants (uid_rfid, nom, prenom, codeGroupe, email) VALUES
# ('04A1B2C3', 'DUPONT', 'Jean', 15824990, 'jean.dupont@example.com');
```

### Étape 5 : Configurer le serveur PHP

```bash
# Copier le fichier PHP sur votre serveur web
scp server/export_etudiants.php user@votre-serveur.com:/var/www/html/

# Éditer la configuration (lignes 11-14)
# define('DB_HOST', 'localhost');
# define('DB_NAME', 'DST_DB');
# define('DB_USER', 'votre_utilisateur');
# define('DB_PASS', 'votre_mot_de_passe');
```

### Étape 6 : Configurer l'Arduino

Éditez [src/main.cpp](src/main.cpp) (lignes 9-15) :

```cpp
// Configuration WiFi
const char WIFI_SSID[] = "VotreSSID";
const char WIFI_PASSWORD[] = "VotreMotDePasse";

// Configuration serveur
const char SERVER_HOST[] = "votre-serveur.com";
const int SERVER_PORT = 80;  // ou 443 pour HTTPS
const char SERVER_PATH[] = "/export_etudiants.php";
```

### Étape 7 : Compiler et téléverser

```bash
# Compiler
./scripts/build.sh

# Téléverser sur l'Arduino
./scripts/upload.sh

# Ouvrir le moniteur série
./scripts/monitor.sh
```

## Documentation détaillée

Pour plus d'informations, consultez la documentation complète :

- **[INSTALLATION.md](docs/INSTALLATION.md)** - Guide d'installation pas à pas
- **[QUICKSTART.md](docs/QUICKSTART.md)** - Démarrage rapide (5 minutes)
- **[VERIFICATION.md](docs/VERIFICATION.md)** - Tests et vérifications
- **[ENVIRONNEMENT_VIRTUEL.md](docs/ENVIRONNEMENT_VIRTUEL.md)** - Environnement virtuel Python
- **[CONTRIBUTING.md](docs/CONTRIBUTING.md)** - Comment contribuer
- **[SUMMARY.md](docs/SUMMARY.md)** - Résumé complet du projet
- **[PROJECT_STATUS.md](docs/PROJECT_STATUS.md)** - État actuel du projet
- **[FILES_GUIDE.md](docs/FILES_GUIDE.md)** - Guide détaillé des fichiers

## Utilisation

### Démarrage du système

1. Insérez la carte SD formatée en FAT32
2. Alimentez l'Arduino via USB ou alimentation externe
3. Le système effectue automatiquement :
   - Connexion au WiFi
   - Téléchargement de la base de données
   - Initialisation du lecteur RFID
   - Création des fichiers CSV

### Badgeage

1. Présentez une carte RFID devant le lecteur
2. Le système affiche :
   - L'UID de la carte détectée
   - Les informations de l'étudiant (si trouvé)
   - La confirmation d'enregistrement

### Exemple de sortie série

```
========================================
  Système de Badgeuse RFID avec WiFi
========================================
[SD] Carte SD initialisée
[RFID] Lecteur initialisé
[WiFi] Connexion à MonWiFi... Connecté!
[WiFi] Adresse IP: 192.168.1.42
[DB] Téléchargement de la base depuis votre-serveur.com
[DB] Code HTTP: 200
[DB] Base téléchargée: 1245 octets
[DB] Base de données prête

[SYSTÈME] Prêt - Présentez un badge

----------------------------------------
[RFID] Badge détecté: 04A1B2C3
[DB] Étudiant trouvé: Jean DUPONT
[SD] Badgeage enregistré
----------------------------------------
```

### Fichiers générés sur la carte SD

#### badges.csv

Format : `timestamp_ms,uid_hex,nom,prenom`

```csv
timestamp_ms,uid_hex,nom,prenom
1234567890,04A1B2C3,DUPONT,Jean
1234567950,04D4E5F6,MARTIN,Marie
```

#### etudiants.csv

Base de données locale, format : `uid_rfid,nom,prenom,email,codeGroupe`

```csv
uid_rfid,nom,prenom,email,codeGroupe
04A1B2C3,DUPONT,Jean,jean.dupont@example.com,15824990
04D4E5F6,MARTIN,Marie,marie.martin@example.com,15824990
```

## Configuration

### Paramètres configurables dans [src/main.cpp](src/main.cpp)

```cpp
// Délai anti-rebond (évite les doubles lectures)
constexpr uint32_t READ_COOLDOWN_MS = 1500;  // 1.5 secondes

// Intervalle de mise à jour de la base de données
constexpr uint32_t DB_UPDATE_INTERVAL_MS = 3600000;  // 1 heure

// Noms des fichiers sur la carte SD
constexpr char LOG_FILE_NAME[] = "/badges.csv";
constexpr char DB_FILE_NAME[] = "/etudiants.csv";

// Pins matériel
constexpr uint8_t RFID_SS_PIN = 7;
constexpr uint8_t RFID_RST_PIN = 9;
constexpr uint8_t SD_CS_PIN = 10;
constexpr uint8_t SD_PWR_PIN = 8;
```

### Modification de la fréquence de mise à jour

Pour forcer une mise à jour immédiate, redémarrez l'Arduino. Pour modifier l'intervalle, changez `DB_UPDATE_INTERVAL_MS` :

```cpp
// Mise à jour toutes les 30 minutes
constexpr uint32_t DB_UPDATE_INTERVAL_MS = 1800000;

// Mise à jour toutes les 2 heures
constexpr uint32_t DB_UPDATE_INTERVAL_MS = 7200000;
```

## Maintenance

### Récupération des logs

```bash
# 1. Retirez la carte SD de l'Arduino
# 2. Insérez-la dans votre ordinateur
# 3. Copiez le fichier badges.csv
cp /Volumes/SD_CARD/badges.csv ~/Downloads/

# 4. Importez dans votre base de données ou Excel
```

### Ajout de nouveaux étudiants

```sql
-- 1. Ajouter dans SRC_DB
INSERT INTO etudiants (uid_rfid, nom, prenom, codeGroupe, email) VALUES
('04ABCDEF', 'NOUVEAU', 'Etudiant', 15824990, 'nouveau@example.com');

-- 2. Synchroniser vers DST_DB
SOURCE database/CopybaseSimple.sql;

-- 3. L'Arduino téléchargera automatiquement la mise à jour
```

### Nettoyage du projet

```bash
# Supprimer les environnements et builds
./scripts/clean.sh

# Reconfigurer
./scripts/setup_env.sh
```

## Dépannage

### L'Arduino ne se connecte pas au WiFi

**Symptômes** : Message "Échec de connexion WiFi"

**Solutions** :
- Vérifiez le SSID et le mot de passe dans [src/main.cpp](src/main.cpp)
- Assurez-vous que le réseau est en 2.4 GHz (l'Arduino R4 WiFi ne supporte pas le 5 GHz)
- Vérifiez la portée du signal WiFi
- Essayez de redémarrer votre routeur

### Le téléchargement de la base échoue

**Symptômes** : Message "Échec du téléchargement" ou "Code HTTP: 404"

**Solutions** :
- Testez l'URL dans un navigateur : `http://votre-serveur.com/export_etudiants.php`
- Vérifiez les identifiants MySQL dans [server/export_etudiants.php](server/export_etudiants.php)
- Vérifiez que le serveur autorise les requêtes depuis l'IP de l'Arduino
- Consultez les logs Apache/Nginx : `/var/log/apache2/error.log`

### Les étudiants ne sont pas trouvés

**Symptômes** : "Étudiant non trouvé" malgré une carte valide

**Solutions** :
- Vérifiez que les UIDs correspondent exactement (majuscules, pas d'espaces)
- Lisez `etudiants.csv` sur la carte SD pour vérifier son contenu
- Utilisez le moniteur série pour voir l'UID réel détecté
- Comparez avec la base de données MySQL

### Erreur de carte SD

**Symptômes** : "Échec d'initialisation de la carte SD"

**Solutions** :
- Vérifiez que la carte est formatée en FAT32
- Testez avec une autre carte SD (max 32 Go recommandé)
- Vérifiez le câblage (pins CS=10, PWR=8)
- Nettoyez les contacts de la carte SD

### Erreur de compilation

**Symptômes** : Échec lors de `./scripts/build.sh`

**Solutions** :

```bash
# Vérifier l'installation
./scripts/check.sh

# Nettoyer et reconfigurer
./scripts/clean.sh
./scripts/setup_env.sh

# Vérifier les dépendances
source .venv/bin/activate
pio pkg list
```

### Problème de port série

**Symptômes** : "Device not found" lors de l'upload

**Solutions** :
- Vérifiez que l'Arduino est bien connecté en USB
- Fermez tous les moniteurs série ouverts
- Vérifiez les permissions :
  ```bash
  # macOS/Linux
  sudo chmod 666 /dev/ttyACM0
  # ou
  sudo usermod -a -G dialout $USER
  ```
- Essayez un autre câble USB

## Contribution

Les contributions sont les bienvenues ! Consultez [CONTRIBUTING.md](docs/CONTRIBUTING.md) pour :
- Comment signaler un bug
- Comment proposer une amélioration
- Comment soumettre une pull request
- Standards de code et conventions

## Sécurité

### Avertissements

- Le script PHP ne nécessite pas d'authentification par défaut
- Les mots de passe WiFi sont stockés en clair dans le code Arduino
- La base de données locale sur la carte SD n'est pas chiffrée

### Recommandations

Pour sécuriser le système :

1. **Authentification PHP** : Ajoutez un token dans l'URL
   ```php
   if ($_GET['token'] !== 'votre_token_secret') {
       http_response_code(401);
       exit;
   }
   ```

2. **HTTPS** : Utilisez SSL/TLS pour les communications
   ```cpp
   const int SERVER_PORT = 443;  // HTTPS
   ```

3. **Restriction IP** : Limitez l'accès à l'IP de l'Arduino
   ```apache
   <Files "export_etudiants.php">
       Order Deny,Allow
       Deny from all
       Allow from 192.168.1.42
   </Files>
   ```

## Performances

- **Temps de démarrage** : ~5-10 secondes
- **Temps de lecture RFID** : <500ms
- **Temps d'écriture SD** : ~50ms par badgeage
- **Consommation RAM** : 5960 bytes / 32768 bytes (18.2%)
- **Consommation Flash** : 70300 bytes / 262144 bytes (26.8%)
- **Autonomie** : Alimentation USB ou 7-12V DC

## Limitations connues

- Maximum ~1000 étudiants dans la base de données locale
- Pas de support 5 GHz WiFi (limitation matérielle)
- Pas de RTC (horloge temps réel) : timestamps en millisecondes depuis le démarrage
- Pas de cryptage des données
- Un seul lecteur RFID supporté

## Améliorations futures

- [ ] Ajout d'un écran LCD pour affichage des informations
- [ ] Support de plusieurs lecteurs RFID
- [ ] Module RTC pour timestamps absolus
- [ ] Cryptage des données sur la carte SD
- [ ] Interface web pour consultation des logs
- [ ] Mode AP (Access Point) pour configuration sans recompilation
- [ ] Support NFC (cartes sans contact)
- [ ] Buzzer et LED pour feedback visuel/sonore

## Ressources utiles

- [Documentation Arduino UNO R4 WiFi](https://docs.arduino.cc/hardware/uno-r4-wifi)
- [Bibliothèque MFRC522](https://github.com/miguelbalboa/rfid)
- [PlatformIO Documentation](https://docs.platformio.org/)
- [MySQL Documentation](https://dev.mysql.com/doc/)

## Auteurs

Projet SAE Badgeuse Izly - IUT/Université

## Licence

Projet éducatif - Usage libre pour l'enseignement et la formation

---

**Besoin d'aide ?** Consultez la [documentation complète](docs/) ou ouvrez une issue sur GitHub.
