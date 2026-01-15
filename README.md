# Système de Badgeuse RFID avec Arduino UNO R4 WiFi

Système de badgeage pour étudiants utilisant des cartes RFID, avec synchronisation de base de données via WiFi et stockage local sur carte SD.

## 🚀 Démarrage rapide

```bash
# 1. Cloner le projet
git clone <URL_DU_REPO>
cd SaeBadgeuseIzly

# 2. Setup automatique (crée l'environnement virtuel)
chmod +x *.sh
./setup_env.sh

# 3. Vérifier l'installation
./check.sh

# 4. Compiler
./build.sh
```

**Plus de détails** : Consultez [INSTALLATION.md](INSTALLATION.md)

## Architecture du système

```
Base de données MySQL (SRC_DB)
           ↓
Script PHP (export_etudiants.php)
           ↓ HTTP/WiFi
Arduino UNO R4 WiFi ← Lecteur RFID (RC522)
           ↓
    Carte SD (stockage)
```

## Matériel requis

- Arduino UNO R4 WiFi
- Lecteur RFID RC522
- Module carte SD
- Carte SD formatée en FAT32
- Cartes RFID pour les étudiants
- Câbles de connexion

## Câblage

### Lecteur RFID RC522
| RC522  | Arduino R4 WiFi |
|--------|----------------|
| SDA    | Pin 7          |
| SCK    | SCK (SPI)      |
| MOSI   | MOSI (SPI)     |
| MISO   | MISO (SPI)     |
| IRQ    | Non connecté   |
| GND    | GND            |
| RST    | Pin 9          |
| 3.3V   | 3.3V           |

### Module carte SD
| SD Card | Arduino R4 WiFi |
|---------|----------------|
| CS      | Pin 10         |
| SCK     | SCK (SPI)      |
| MOSI    | MOSI (SPI)     |
| MISO    | MISO (SPI)     |
| VCC     | 5V             |
| GND     | GND            |
| PWR     | Pin 8          |

## Installation

### 1. Configuration de la base de données

#### a) Créer la table des étudiants

Exécutez le fichier `etudiants_rfid.sql` sur votre serveur MySQL :

```bash
mysql -u root -p SRC_DB < etudiants_rfid.sql
```

#### b) Ajouter vos étudiants

Modifiez le fichier SQL ou insérez vos données via phpMyAdmin :

```sql
INSERT INTO etudiants (uid_rfid, nom, prenom, codeGroupe, email) VALUES
('04A1B2C3', 'DUPONT', 'Jean', 15824990, 'jean.dupont@example.com'),
('04D4E5F6', 'MARTIN', 'Marie', 15824990, 'marie.martin@example.com');
```

**Important**: Les UIDs RFID doivent être en MAJUSCULES et sans espaces.

#### c) Mettre à jour CopybaseSimple.sql

Ajoutez à la fin du fichier `CopybaseSimple.sql` :

```sql
-- ÉTUDIANTS
DROP TABLE IF EXISTS `DST_DB`.`etudiants`;
CREATE TABLE `DST_DB`.`etudiants` LIKE `SRC_DB`.`etudiants`;
INSERT INTO  `DST_DB`.`etudiants`
SELECT      e.*
FROM        `SRC_DB`.`etudiants` e
WHERE       (COALESCE(e.deleted,0)=0)
  AND       (e.codeGroupe IS NULL
           OR e.codeGroupe IN (SELECT codeGroupe FROM `DST_DB`.`ressources_groupes`));

ALTER TABLE `DST_DB`.`etudiants` ADD INDEX idx_uid_rfid_lookup (uid_rfid);
```

### 2. Installation du script PHP

#### a) Copier le fichier

Copiez `export_etudiants.php` sur votre serveur web :

```bash
scp export_etudiants.php user@votre-serveur.com:/var/www/html/
```

#### b) Configurer les identifiants

Éditez le fichier et modifiez les constantes :

```php
define('DB_HOST', 'localhost');
define('DB_NAME', 'DST_DB');
define('DB_USER', 'votre_utilisateur');
define('DB_PASS', 'votre_mot_de_passe');
```

#### c) Tester le script

Visitez `https://votre-serveur.com/export_etudiants.php` dans votre navigateur. Vous devriez télécharger un fichier CSV contenant vos étudiants.

### 3. Configuration de l'Arduino

#### a) Modifier les paramètres dans src/main.cpp

```cpp
// Configuration WiFi
const char WIFI_SSID[] = "VotreSSID";
const char WIFI_PASSWORD[] = "VotreMotDePasse";

// Configuration serveur
const char SERVER_HOST[] = "votre-serveur.com";
const int SERVER_PORT = 80;  // ou 443 pour HTTPS
const char SERVER_PATH[] = "/export_etudiants.php";
```

#### b) Compiler et téléverser

```bash
# Avec PlatformIO
pio run --target upload

# Avec PlatformIO CLI
platformio run -e uno_r4_wifi --target upload
```

#### c) Ouvrir le moniteur série

```bash
pio device monitor -b 115200
```

## Utilisation

### Démarrage

1. Insérez une carte SD formatée en FAT32
2. Alimentez l'Arduino
3. Le système va :
   - Se connecter au WiFi
   - Télécharger la base de données
   - Initialiser le lecteur RFID

### Badgeage

1. Présentez une carte RFID devant le lecteur
2. Le système affiche :
   - L'UID de la carte
   - Le nom et prénom de l'étudiant (si trouvé)
   - La confirmation de l'enregistrement

### Exemple de sortie

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

## Fichiers générés sur la carte SD

### badges.csv

Format : `timestamp_ms,uid_hex,nom,prenom`

```csv
timestamp_ms,uid_hex,nom,prenom
1234567,04A1B2C3,DUPONT,Jean
1245678,04D4E5F6,MARTIN,Marie
```

### etudiants.csv

Base de données locale téléchargée, format : `uid_rfid,nom,prenom,email,codeGroupe`

## Maintenance

### Mise à jour de la base de données

La base se met à jour automatiquement toutes les heures. Pour forcer une mise à jour :
- Redémarrez l'Arduino
- Ou modifiez `DB_UPDATE_INTERVAL_MS` dans le code

### Récupération des logs

1. Retirez la carte SD de l'Arduino
2. Lisez le fichier `badges.csv` sur votre ordinateur
3. Importez dans Excel, LibreOffice, ou votre base de données

### Ajout de nouveaux étudiants

1. Ajoutez l'étudiant dans la table `etudiants` de SRC_DB
2. Exécutez `CopybaseSimple.sql` pour synchroniser DST_DB
3. L'Arduino téléchargera automatiquement la mise à jour

## Dépannage

### L'Arduino ne se connecte pas au WiFi

- Vérifiez le SSID et le mot de passe
- Vérifiez que le réseau WiFi est en 2.4 GHz (l'Arduino R4 WiFi ne supporte pas le 5 GHz)
- Vérifiez la portée du signal WiFi

### Le téléchargement de la base échoue

- Testez l'URL dans un navigateur
- Vérifiez les identifiants de la base de données dans le PHP
- Vérifiez que le serveur autorise les requêtes HTTP depuis l'IP de l'Arduino

### Les étudiants ne sont pas trouvés

- Vérifiez que les UIDs dans la base correspondent exactement (majuscules, pas d'espaces)
- Lisez le fichier `etudiants.csv` sur la carte SD pour vérifier son contenu
- Utilisez le moniteur série pour voir l'UID réel détecté

### Erreur de carte SD

- Vérifiez que la carte est formatée en FAT32
- Vérifiez le câblage
- Testez avec une autre carte SD

## Paramètres configurables

Dans [src/main.cpp](src/main.cpp) :

```cpp
// Délai anti-rebond pour éviter les doubles lectures
constexpr uint32_t READ_COOLDOWN_MS = 1500;  // 1.5 secondes

// Intervalle de mise à jour de la base
constexpr uint32_t DB_UPDATE_INTERVAL_MS = 3600000;  // 1 heure

// Fichiers sur la carte SD
constexpr char LOG_FILE_NAME[] = "/badges.csv";
constexpr char DB_FILE_NAME[] = "/etudiants.csv";
```

## Sécurité

- Le script PHP ne nécessite pas d'authentification par défaut
- Pour sécuriser, ajoutez une authentification HTTP Basic ou un token dans l'URL
- Les mots de passe WiFi sont stockés en clair dans le code Arduino
- La base de données locale sur la carte SD n'est pas chiffrée

## Licence

Projet éducatif - IUT/Université

## Auteurs

Projet SAE Badgeuse Izly
