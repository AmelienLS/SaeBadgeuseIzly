#!/bin/bash

# ============================================
# Script de téléversement simplifié
# Compile et téléverse sur l'Arduino
# ============================================

set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)
VENV_DIR="$PROJECT_DIR/.venv"

cd "$PROJECT_DIR"

# Vérifier que l'environnement virtuel existe
if [ ! -d "$VENV_DIR" ]; then
    echo "❌ Environnement virtuel non trouvé"
    echo "   Lancez d'abord: ./setup_env.sh"
    exit 1
fi

# Activer l'environnement virtuel
source "$VENV_DIR/bin/activate"

echo "======================================"
echo "  Téléversement sur Arduino"
echo "======================================"
echo ""
echo "⚠️  Assurez-vous que:"
echo "    - L'Arduino est branché en USB"
echo "    - Aucun autre programme n'utilise le port série"
echo ""
read -p "Continuer ? (o/N) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Oo]$ ]]; then
    echo "Annulé"
    exit 0
fi

echo ""
echo "🔨 Compilation et téléversement..."
pio run -e uno_r4_wifi --target upload

echo ""
echo "======================================"
echo "  ✅ Téléversement réussi !"
echo "======================================"
echo ""
echo "L'Arduino redémarre automatiquement."
echo ""
echo "Pour voir les messages:"
echo "   ./scripts/monitor.sh"
echo ""
