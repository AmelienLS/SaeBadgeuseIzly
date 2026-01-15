#!/bin/bash

# ============================================
# Script de monitoring série simplifié
# Affiche les messages de l'Arduino
# ============================================

set -e

VENV_DIR=".venv"

# Vérifier que l'environnement virtuel existe
if [ ! -d "$VENV_DIR" ]; then
    echo "❌ Environnement virtuel non trouvé"
    echo "   Lancez d'abord: ./setup_env.sh"
    exit 1
fi

# Activer l'environnement virtuel
source "$VENV_DIR/bin/activate"

echo "======================================"
echo "  Moniteur Série Arduino"
echo "======================================"
echo ""
echo "📡 Connexion au port série (115200 baud)..."
echo ""
echo "💡 Pour quitter: Ctrl+C"
echo ""
echo "--------------------------------------"
echo ""

# Lancer le moniteur série
pio device monitor -b 115200
