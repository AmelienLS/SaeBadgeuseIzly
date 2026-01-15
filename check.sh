#!/bin/bash

# ============================================
# Script de vérification de l'installation
# Vérifie que tout est correctement configuré
# ============================================

VENV_DIR=".venv"
ERROR_COUNT=0

echo "======================================"
echo "  Vérification de l'installation"
echo "======================================"
echo ""

# Fonction pour afficher les résultats
check_item() {
    if [ $1 -eq 0 ]; then
        echo "✅ $2"
    else
        echo "❌ $2"
        ERROR_COUNT=$((ERROR_COUNT + 1))
    fi
}

# 1. Vérifier Python 3
echo "🔍 Vérification des prérequis..."
echo ""

if command -v python3 &> /dev/null; then
    PYTHON_VERSION=$(python3 --version 2>&1)
    check_item 0 "Python 3 installé : $PYTHON_VERSION"
else
    check_item 1 "Python 3 n'est pas installé"
fi

# 2. Vérifier Git
if command -v git &> /dev/null; then
    GIT_VERSION=$(git --version 2>&1)
    check_item 0 "Git installé : $GIT_VERSION"
else
    check_item 1 "Git n'est pas installé"
fi

echo ""
echo "🔍 Vérification de l'environnement virtuel..."
echo ""

# 3. Vérifier l'environnement virtuel
if [ -d "$VENV_DIR" ]; then
    check_item 0 "Environnement virtuel existe : $VENV_DIR/"

    # Vérifier que Python est dans le venv
    if [ -f "$VENV_DIR/bin/python" ]; then
        check_item 0 "Python dans l'environnement virtuel"
    else
        check_item 1 "Python manquant dans l'environnement virtuel"
    fi

    # Vérifier PlatformIO
    if [ -f "$VENV_DIR/bin/pio" ]; then
        PIO_VERSION=$("$VENV_DIR/bin/pio" --version 2>&1 | head -n 1)
        check_item 0 "PlatformIO installé : $PIO_VERSION"
    else
        check_item 1 "PlatformIO n'est pas installé"
    fi
else
    check_item 1 "Environnement virtuel n'existe pas : $VENV_DIR/"
fi

echo ""
echo "🔍 Vérification des fichiers du projet..."
echo ""

# 4. Vérifier les fichiers essentiels
[ -f "platformio.ini" ] && check_item 0 "platformio.ini existe" || check_item 1 "platformio.ini manquant"
[ -f "src/main.cpp" ] && check_item 0 "src/main.cpp existe" || check_item 1 "src/main.cpp manquant"
[ -f "requirements.txt" ] && check_item 0 "requirements.txt existe" || check_item 1 "requirements.txt manquant"

echo ""
echo "🔍 Vérification des scripts..."
echo ""

# 5. Vérifier que les scripts sont exécutables
[ -x "setup_env.sh" ] && check_item 0 "setup_env.sh est exécutable" || check_item 1 "setup_env.sh n'est pas exécutable"
[ -x "build.sh" ] && check_item 0 "build.sh est exécutable" || check_item 1 "build.sh n'est pas exécutable"
[ -x "upload.sh" ] && check_item 0 "upload.sh est exécutable" || check_item 1 "upload.sh n'est pas exécutable"
[ -x "monitor.sh" ] && check_item 0 "monitor.sh est exécutable" || check_item 1 "monitor.sh n'est pas exécutable"

echo ""
echo "🔍 Vérification de la configuration..."
echo ""

# 6. Vérifier la configuration dans main.cpp
if grep -q "VOTRE_SSID" src/main.cpp 2>/dev/null; then
    check_item 1 "WiFi SSID n'est pas configuré (toujours 'VOTRE_SSID')"
else
    check_item 0 "WiFi SSID semble configuré"
fi

if grep -q "votre-serveur.com" src/main.cpp 2>/dev/null; then
    check_item 1 "Serveur n'est pas configuré (toujours 'votre-serveur.com')"
else
    check_item 0 "Serveur semble configuré"
fi

echo ""
echo "======================================"

if [ $ERROR_COUNT -eq 0 ]; then
    echo "  ✅ Installation complète et valide !"
    echo "======================================"
    echo ""
    echo "Vous pouvez maintenant :"
    echo "  ./build.sh    - Compiler le projet"
    echo "  ./upload.sh   - Téléverser sur l'Arduino"
    echo "  ./monitor.sh  - Moniteur série"
    echo ""
    exit 0
else
    echo "  ⚠️  $ERROR_COUNT erreur(s) détectée(s)"
    echo "======================================"
    echo ""
    if [ ! -d "$VENV_DIR" ]; then
        echo "💡 Lancez d'abord : ./setup_env.sh"
    elif [ ! -x "setup_env.sh" ]; then
        echo "💡 Rendez les scripts exécutables : chmod +x *.sh"
    fi
    echo ""
    exit 1
fi
