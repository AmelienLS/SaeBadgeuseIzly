#!/bin/bash

# ============================================
# Script de compilation simplifié
# Utilise l'environnement virtuel local
# ============================================

set -e

VENV_DIR=".venv"
PROJECT_DIR=$(cd "$(dirname "$0")" && pwd)

# Vérifier que l'environnement virtuel existe
if [ ! -d "$VENV_DIR" ]; then
    echo "❌ Environnement virtuel non trouvé"
    echo "   Lancez d'abord: ./setup_env.sh"
    exit 1
fi

# Activer l'environnement virtuel
source "$VENV_DIR/bin/activate"

# Afficher la configuration
echo "======================================"
echo "  Compilation du projet"
echo "======================================"
echo ""
echo "Environnement: $VIRTUAL_ENV"
echo "PlatformIO: $(pio --version)"
echo "Board: Arduino UNO R4 WiFi"
echo ""

# Compiler
echo "🔨 Compilation en cours..."
pio run -e uno_r4_wifi

echo ""
echo "======================================"
echo "  ✅ Compilation réussie !"
echo "======================================"
echo ""
echo "Pour téléverser:"
echo "   ./upload.sh"
echo ""
echo "Pour le moniteur série:"
echo "   ./monitor.sh"
echo ""
