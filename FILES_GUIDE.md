# Guide des Fichiers du Projet

Référence rapide de tous les fichiers du projet et leur utilité.

## 📂 Structure Complète

```
SaeBadgeuseIzly/
│
├── 📁 src/                          Code source
│   └── main.cpp                    Code Arduino principal (474 lignes)
│
├── 📁 scripts/                      Scripts de build
│   └── fix_path.py                 Fixe le PATH pour compilation macOS
│
├── 🔧 Configuration
│   ├── platformio.ini              Configuration PlatformIO
│   ├── requirements.txt            Dépendances Python
│   ├── .gitignore                  Fichiers à ignorer par Git
│   └── .gitattributes              Gestion des fins de ligne
│
├── 🚀 Scripts Shell
│   ├── setup_env.sh                Configuration automatique
│   ├── build.sh                    Compilation rapide
│   ├── upload.sh                   Téléversement sur Arduino
│   ├── monitor.sh                  Moniteur série
│   ├── clean.sh                    Nettoyage du projet
│   └── check.sh                    Vérification de l'installation
│
├── 🗄️ Base de Données
│   ├── etudiants_rfid.sql          Création table étudiants
│   ├── CopybaseSimple_etudiants_addon.sql  Addon pour synchronisation
│   ├── CopybaseSimple.sql          Script de copie complet (existant)
│   ├── Base_Seances.sql            Base de séances (existant)
│   ├── complete_SRC_DB.sql         Structure complète (existant)
│   └── export_etudiants.php        API d'export CSV
│
└── 📖 Documentation
    ├── README.md                   Documentation principale
    ├── INSTALLATION.md             Guide d'installation complet
    ├── QUICKSTART.md               Démarrage en 5 minutes
    ├── VERIFICATION.md             Vérification et dépannage
    ├── ENVIRONNEMENT_VIRTUEL.md    Documentation env virtuel
    ├── CONTRIBUTING.md             Guide de contribution
    ├── SUMMARY.md                  Résumé du projet
    ├── PROJECT_STATUS.md           État actuel du projet
    └── FILES_GUIDE.md              Ce fichier
```

---

## 📝 Fichiers par Catégorie

### 🔴 ESSENTIELS (ne pas supprimer)

| Fichier | Description | Modifiable ? |
|---------|-------------|--------------|
| `src/main.cpp` | Code Arduino principal | ✅ OUI |
| `platformio.ini` | Configuration PlatformIO | ⚠️ Avec précaution |
| `requirements.txt` | Dépendances Python | ⚠️ Rarement |

### 🟢 SCRIPTS UTILITAIRES

| Script | Fonction | Quand l'utiliser ? |
|--------|----------|--------------------|
| `setup_env.sh` | Setup initial | Une fois au début |
| `build.sh` | Compiler | À chaque modification |
| `upload.sh` | Téléverser | Après compilation réussie |
| `monitor.sh` | Voir les logs | Pour déboguer |
| `check.sh` | Vérifier setup | En cas de problème |
| `clean.sh` | Tout nettoyer | Pour recommencer |

### 🔵 CONFIGURATION GIT

| Fichier | Rôle |
|---------|------|
| `.gitignore` | Empêche de commit `.venv/`, `.pio/`, logs |
| `.gitattributes` | Gère les fins de ligne (LF) |

### 🟡 BASE DE DONNÉES

| Fichier | Contenu |
|---------|---------|
| `etudiants_rfid.sql` | Création table étudiants + exemples |
| `CopybaseSimple_etudiants_addon.sql` | À ajouter à CopybaseSimple.sql |
| `export_etudiants.php` | API pour exporter en CSV |

### 🟣 DOCUMENTATION

| Fichier | Pour qui ? |
|---------|-----------|
| `QUICKSTART.md` | Débutants (5 min) |
| `INSTALLATION.md` | Guide complet |
| `README.md` | Vue d'ensemble |
| `VERIFICATION.md` | En cas de problème |
| `ENVIRONNEMENT_VIRTUEL.md` | Détails techniques env |
| `CONTRIBUTING.md` | Contributeurs |
| `SUMMARY.md` | Résumé complet |
| `PROJECT_STATUS.md` | État actuel |

---

## 🎯 Fichiers par Besoin

### "Je veux compiler le projet"
1. `README.md` - Vue d'ensemble
2. `QUICKSTART.md` - Démarrage rapide
3. `setup_env.sh` - Configuration
4. `build.sh` - Compilation

### "J'ai un problème"
1. `VERIFICATION.md` - Dépannage
2. `check.sh` - Vérification auto
3. `PROJECT_STATUS.md` - État du projet

### "Je veux contribuer"
1. `CONTRIBUTING.md` - Guide complet
2. `INSTALLATION.md` - Setup détaillé
3. `.gitignore` - Fichiers à ne pas commit

### "Je veux configurer la base de données"
1. `etudiants_rfid.sql` - Créer la table
2. `CopybaseSimple_etudiants_addon.sql` - Synchronisation
3. `export_etudiants.php` - API d'export

### "Je veux comprendre le code"
1. `src/main.cpp` - Code Arduino
2. `README.md` - Architecture
3. `SUMMARY.md` - Vue détaillée

---

## 🚫 Fichiers IGNORÉS par Git

Ces fichiers/dossiers ne seront **jamais** commités :

```
.venv/              # Environnement virtuel Python
.pio/               # Builds et caches PlatformIO
*.log               # Fichiers de logs
*.o, *.bin, *.elf   # Fichiers compilés
__pycache__/        # Cache Python
.DS_Store           # Fichiers macOS
credentials.h       # Secrets (si créé)
```

**Pourquoi ?**
- `.venv/` : 100+ MB, chaque dev a le sien
- `.pio/` : 50+ MB, rebuilds à chaque fois
- Logs : Inutiles sur GitHub

---

## 📏 Taille des Fichiers

### Code Source
- `src/main.cpp` : ~15 KB (474 lignes)
- `export_etudiants.php` : ~2 KB (62 lignes)
- `*.sql` : ~5 KB total

### Documentation
- Tous les `.md` : ~100 KB total
- 7 fichiers, ~2500 lignes

### Scripts
- Tous les `.sh` : ~15 KB total
- 6 scripts utilitaires

**Total du repo (sans .venv/.pio)** : ~130 KB

---

## 🔍 Fichiers à Modifier Avant Déploiement

### OBLIGATOIRE

1. **src/main.cpp** (lignes 9-15)
```cpp
const char WIFI_SSID[] = "VotreSSID";           // ← À MODIFIER
const char WIFI_PASSWORD[] = "VotrePassword";    // ← À MODIFIER
const char SERVER_HOST[] = "votre-serveur.com";  // ← À MODIFIER
```

2. **export_etudiants.php** (lignes 11-14)
```php
define('DB_HOST', 'localhost');      // ← À MODIFIER
define('DB_NAME', 'DST_DB');         // ← À MODIFIER
define('DB_USER', 'utilisateur');    // ← À MODIFIER
define('DB_PASS', 'mot_de_passe');   // ← À MODIFIER
```

### OPTIONNEL

- `platformio.ini` : Changer la carte si nécessaire
- `*.sql` : Ajouter vos étudiants

---

## 📦 Fichiers Générés Automatiquement

### Par PlatformIO (dans .pio/)
```
.pio/build/          # Fichiers compilés
.pio/libdeps/        # Dépendances téléchargées
```

### Par Python (dans .venv/)
```
.venv/bin/           # Exécutables Python/PlatformIO
.venv/lib/           # Bibliothèques Python
```

### Par le projet
```
activate.sh          # Créé par setup_env.sh
badges.csv           # Créé sur la carte SD
```

**Ces fichiers sont ignorés par Git**

---

## 🎓 Ordre de Lecture Recommandé

### Pour un débutant
1. `QUICKSTART.md` (5 min)
2. `README.md` (10 min)
3. `src/main.cpp` (parcourir)

### Pour un développeur
1. `INSTALLATION.md` (installation)
2. `CONTRIBUTING.md` (conventions)
3. `src/main.cpp` (code complet)
4. `VERIFICATION.md` (dépannage)

### Pour un admin système
1. `README.md` (vue d'ensemble)
2. `etudiants_rfid.sql` (base de données)
3. `export_etudiants.php` (API)
4. `src/main.cpp` (lignes 9-15 pour config)

---

## 🔑 Fichiers Clés par Rôle

### Développeur Arduino
- `src/main.cpp` ⭐⭐⭐
- `platformio.ini` ⭐⭐
- `build.sh` ⭐⭐⭐

### Développeur Web/Backend
- `export_etudiants.php` ⭐⭐⭐
- `etudiants_rfid.sql` ⭐⭐⭐
- `CopybaseSimple_etudiants_addon.sql` ⭐⭐

### DevOps / Admin Système
- `setup_env.sh` ⭐⭐⭐
- `.gitignore` ⭐⭐
- `requirements.txt` ⭐⭐

### Chef de Projet
- `PROJECT_STATUS.md` ⭐⭐⭐
- `SUMMARY.md` ⭐⭐⭐
- `README.md` ⭐⭐

---

## 💡 Conseils

### À faire
- ✅ Lire `QUICKSTART.md` en premier
- ✅ Utiliser les scripts shell (`./build.sh`)
- ✅ Consulter `check.sh` en cas de doute
- ✅ Lire les commentaires dans le code

### À éviter
- ❌ Modifier `platformio.ini` sans comprendre
- ❌ Supprimer `.gitignore`
- ❌ Commit `.venv/` ou `.pio/`
- ❌ Modifier `fix_path.py` sans raison

---

## 📊 Statistiques des Fichiers

| Type | Nombre | Taille totale |
|------|--------|---------------|
| Code source | 4 | ~22 KB |
| Scripts | 7 | ~15 KB |
| Documentation | 9 | ~100 KB |
| Configuration | 4 | ~5 KB |
| **Total** | **24** | **~142 KB** |

---

## 🎯 Résumé

### Fichiers les plus importants
1. `src/main.cpp` - Le cœur du projet
2. `platformio.ini` - Configuration de build
3. `QUICKSTART.md` - Pour démarrer vite

### Ne jamais supprimer
- `src/main.cpp`
- `platformio.ini`
- `requirements.txt`
- `.gitignore`

### Peuvent être régénérés
- `.venv/` (via `setup_env.sh`)
- `.pio/` (via compilation)
- `activate.sh` (via `setup_env.sh`)

---

**Ce guide référence tous les fichiers du projet et leur utilité ! 📚**
