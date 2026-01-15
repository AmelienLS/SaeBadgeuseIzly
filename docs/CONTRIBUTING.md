# Guide de Contribution

Merci de contribuer au projet SAE Badgeuse RFID ! 🎉

## Mise en place de l'environnement de développement

### 1. Fork et clone

```bash
# Fork le projet sur GitHub, puis :
git clone https://github.com/VOTRE_USERNAME/SaeBadgeuseIzly.git
cd SaeBadgeuseIzly
```

### 2. Configuration

```bash
# Rendre les scripts exécutables
chmod +x *.sh

# Setup automatique
./setup_env.sh

# Vérifier l'installation
./check.sh
```

### 3. Créer une branche

```bash
# Toujours créer une branche pour vos modifications
git checkout -b feature/ma-nouvelle-fonctionnalite
```

## Workflow de développement

### 1. Faire vos modifications

Éditez les fichiers nécessaires :
- `src/main.cpp` pour le code Arduino
- `export_etudiants.php` pour l'API PHP
- `*.sql` pour les modifications de base de données

### 2. Tester vos modifications

```bash
# Compiler
./build.sh

# Si vous avez un Arduino connecté
./upload.sh
./monitor.sh
```

### 3. Commit

```bash
# Ajouter les fichiers modifiés
git add .

# Commit avec un message descriptif
git commit -m "feat: description de la fonctionnalité"
```

### Convention de nommage des commits

- `feat:` - Nouvelle fonctionnalité
- `fix:` - Correction de bug
- `docs:` - Documentation
- `refactor:` - Refactoring sans changement de comportement
- `test:` - Ajout de tests
- `chore:` - Maintenance (dépendances, config, etc.)

**Exemples** :
```bash
git commit -m "feat: ajout de la détection de carte invalide"
git commit -m "fix: correction du parsing CSV avec virgules dans le nom"
git commit -m "docs: mise à jour du guide d'installation"
```

### 4. Push et Pull Request

```bash
# Push vers votre fork
git push origin feature/ma-nouvelle-fonctionnalite
```

Puis créez une Pull Request sur GitHub depuis votre fork vers le repo principal.

## Règles de code

### Code Arduino (C++)

- ✅ Utiliser `F()` pour les chaînes constantes (économie de RAM)
- ✅ Fermer systématiquement les fichiers SD après lecture/écriture
- ✅ Commenter les sections complexes
- ✅ Respecter l'indentation (2 espaces)
- ❌ Pas d'allocation dynamique inutile
- ❌ Pas de `String` dans les boucles critiques (préférer char[])

**Exemple** :
```cpp
// ✅ BON
Serial.println(F("Message constant"));

// ❌ MAUVAIS
Serial.println("Message constant");
```

### Code PHP

- ✅ Utiliser PDO pour la base de données
- ✅ Échapper les sorties (protection XSS)
- ✅ Valider les entrées
- ✅ Gestion d'erreurs avec try/catch
- ❌ Jamais de `mysql_*` (obsolète)
- ❌ Pas de secrets en dur dans le code

### SQL

- ✅ Utiliser des noms de colonnes explicites
- ✅ Ajouter des index sur les colonnes de recherche
- ✅ Utiliser `COALESCE` pour les valeurs nullables
- ✅ Préfixer les requêtes complexes avec des commentaires

## Fichiers à ne JAMAIS commit

Le `.gitignore` empêche déjà de commit :
- `.venv/` - Environnement virtuel
- `.pio/` - Builds PlatformIO
- `credentials.h`, `secrets.h` - Credentials
- `*.log` - Logs

**⚠️ ATTENTION** : Ne commitez JAMAIS de mots de passe ou credentials !

## Tests

Avant de soumettre une PR, vérifiez :

```bash
# 1. Le code compile
./build.sh

# 2. L'installation est valide
./check.sh

# 3. Les scripts sont exécutables
ls -la *.sh

# 4. Le .gitignore fonctionne
git status  # .venv/ et .pio/ ne doivent PAS apparaître
```

## Structure des Pull Requests

### Titre

Format : `[TYPE] Courte description`

Exemples :
- `[FEAT] Ajout du support HTTPS pour le téléchargement`
- `[FIX] Correction du bug de double lecture RFID`
- `[DOCS] Amélioration du guide d'installation`

### Description

Incluez :
1. **Problème résolu** : Quel problème cette PR résout-elle ?
2. **Solution** : Comment le problème est-il résolu ?
3. **Tests** : Comment avez-vous testé ?
4. **Breaking changes** : Y a-t-il des changements incompatibles ?

**Template** :
```markdown
## Problème
Description du problème ou de la fonctionnalité manquante

## Solution
Explication de la solution implémentée

## Tests effectués
- [ ] Compilation réussie avec ./build.sh
- [ ] Testé sur Arduino UNO R4 WiFi
- [ ] Moniteur série vérifié

## Changements incompatibles
Aucun / Liste des breaking changes
```

## Checklist avant de soumettre

- [ ] Le code compile sans erreur (`./build.sh`)
- [ ] Les scripts sont testés
- [ ] La documentation est à jour
- [ ] Les commits suivent la convention de nommage
- [ ] Pas de fichiers sensibles (passwords, credentials)
- [ ] Le `.gitignore` est respecté
- [ ] Les commentaires expliquent le "pourquoi", pas le "quoi"

## Review process

1. **Soumission** : Créez votre PR sur GitHub
2. **Review** : Un mainteneur reviendra votre code
3. **Feedback** : Répondez aux commentaires et faites les modifications
4. **Approbation** : Une fois approuvée, votre PR sera mergée
5. **Merge** : Votre contribution fait maintenant partie du projet ! 🎉

## Questions ?

- Ouvrez une **Issue** pour discuter d'une nouvelle fonctionnalité
- Consultez les **Pull Requests existantes** pour voir des exemples
- Lisez [INSTALLATION.md](INSTALLATION.md) et [VERIFICATION.md](VERIFICATION.md)

## Licence

En contribuant, vous acceptez que vos contributions soient sous la même licence que le projet.

Merci pour votre contribution ! 🙏
