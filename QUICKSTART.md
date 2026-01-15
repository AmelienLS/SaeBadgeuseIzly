# Démarrage Ultra-Rapide ⚡

Guide pour mettre en place le projet en **moins de 5 minutes**.

## Pour un nouveau développeur

### Étape 1 : Récupérer le projet (30 secondes)

```bash
git clone <URL_DU_REPO>
cd SaeBadgeuseIzly
```

### Étape 2 : Installation automatique (2-3 minutes)

```bash
chmod +x *.sh
./setup_env.sh
```

### Étape 3 : Vérification (10 secondes)

```bash
./check.sh
```

### Étape 4 : Test de compilation (30 secondes)

```bash
./build.sh
```

**C'est prêt ! ✅**

## Pour pousser sur GitHub

### Premier push

```bash
# Vérifier que .venv/ n'est pas tracké
git status
# .venv/ ne doit PAS apparaître

# Ajouter les fichiers
git add .

# Commit
git commit -m "Initial commit"

# Push
git push origin main
```

### Push suivants

```bash
git add .
git commit -m "Description des changements"
git push
```

## Scripts disponibles

| Script | Description |
|--------|-------------|
| `./setup_env.sh` | Configuration initiale (une fois) |
| `./check.sh` | Vérifier l'installation |
| `./build.sh` | Compiler le projet |
| `./upload.sh` | Téléverser sur l'Arduino |
| `./monitor.sh` | Moniteur série |
| `./clean.sh` | Nettoyer et recommencer |

## Configuration minimale

Avant de téléverser sur l'Arduino, modifiez dans `src/main.cpp` :

```cpp
// Lignes 9-10
const char WIFI_SSID[] = "VotreSSID";
const char WIFI_PASSWORD[] = "VotrePassword";

// Ligne 13
const char SERVER_HOST[] = "votre-serveur.com";
```

## Fichiers importants

- 📘 [INSTALLATION.md](INSTALLATION.md) - Guide complet d'installation
- 🔍 [VERIFICATION.md](VERIFICATION.md) - Vérification et dépannage
- 🐍 [ENVIRONNEMENT_VIRTUEL.md](ENVIRONNEMENT_VIRTUEL.md) - Doc sur l'env virtuel
- 🤝 [CONTRIBUTING.md](CONTRIBUTING.md) - Guide de contribution
- 📖 [README.md](README.md) - Documentation générale

## FAQ Rapide

### Q: `.venv/` apparaît dans `git status`
**R:** Non, `.gitignore` l'empêche. Si oui, il y a un problème.

### Q: Erreur "Permission denied" sur les scripts
**R:** `chmod +x *.sh`

### Q: L'environnement ne s'active pas
**R:** `rm -rf .venv && ./setup_env.sh`

### Q: La compilation échoue
**R:** `./clean.sh && ./setup_env.sh && ./build.sh`

### Q: Comment partager le projet ?
**R:** Juste push sur GitHub, `.gitignore` s'occupe du reste

## Ce qui est ignoré par Git

✅ **Ignoré** (ne sera jamais committé) :
- `.venv/` - Environnement virtuel Python
- `.pio/` - Builds et caches PlatformIO
- `*.log` - Fichiers de logs
- `*.o`, `*.bin`, `*.elf` - Fichiers compilés
- `credentials.h`, `secrets.h` - Fichiers de secrets

✅ **Committé** (sera sur GitHub) :
- `src/main.cpp` - Code source
- `*.sh` - Scripts utilitaires
- `platformio.ini` - Configuration
- `requirements.txt` - Dépendances Python
- `*.md` - Documentation
- `*.sql`, `*.php` - Base de données et API

## Commandes essentielles

```bash
# Compiler
./build.sh

# Vérifier l'installation
./check.sh

# Nettoyer tout
./clean.sh

# Recommencer
./setup_env.sh

# Status Git (vérifier que .venv/ n'apparaît pas)
git status

# Activer l'environnement manuellement
source .venv/bin/activate
# ... commandes pio ...
deactivate
```

## Résumé du workflow

```
1. git clone <URL>
2. chmod +x *.sh
3. ./setup_env.sh
4. Éditer src/main.cpp (WiFi, serveur)
5. ./build.sh
6. ./upload.sh (Arduino branché)
7. ./monitor.sh (voir les logs)
8. git add .
9. git commit -m "..."
10. git push
```

## Support

- 📖 Lisez d'abord [INSTALLATION.md](INSTALLATION.md)
- 🔍 Consultez [VERIFICATION.md](VERIFICATION.md)
- ❓ Ouvrez une Issue sur GitHub

---

**Temps total estimé : < 5 minutes** ⚡
