#!/bin/bash

# ============================================
# Script de nettoyage du projet
# Supprime les environnements et builds
# ============================================

set -e

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)
PROJECT_DIR=$(cd "$SCRIPT_DIR/.." && pwd)

cd "$PROJECT_DIR"

echo "======================================"
echo "  Nettoyage du projet"
echo "======================================"
echo ""
echo "Ce script va supprimer :"
echo "  - .venv/         (environnement virtuel)"
echo "  - .pio/          (builds PlatformIO)"
echo "  - *.log          (fichiers de logs)"
echo ""

read -p "Continuer ? (o/N) " -n 1 -r
echo ""

if [[ ! $REPLY =~ ^[Oo]$ ]]; then
    echo "Nettoyage annulé"
    exit 0
fi

echo ""
echo "🗑️  Nettoyage en cours..."

# Supprimer l'environnement virtuel
if [ -d ".venv" ]; then
    rm -rf .venv
    echo "✅ .venv/ supprimé"
else
    echo "⚠️  .venv/ n'existe pas"
fi

# Supprimer les builds PlatformIO
if [ -d ".pio" ]; then
    rm -rf .pio
    echo "✅ .pio/ supprimé"
else
    echo "⚠️  .pio/ n'existe pas"
fi

# Supprimer les fichiers de logs
if ls *.log 1> /dev/null 2>&1; then
    rm -f *.log
    echo "✅ Fichiers *.log supprimés"
fi

# Supprimer les fichiers Python compilés
if [ -d "__pycache__" ]; then
    rm -rf __pycache__
    echo "✅ __pycache__/ supprimé"
fi

if [ -d "scripts/__pycache__" ]; then
    rm -rf scripts/__pycache__
    echo "✅ scripts/__pycache__/ supprimé"
fi

echo ""
echo "======================================"
echo "  ✅ Nettoyage terminé !"
echo "======================================"
echo ""
echo "Pour reconfigurer l'environnement :"
echo "  ./scripts/setup_env.sh"
echo ""
