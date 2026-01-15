#!/bin/bash

# ============================================
# Script de configuration d'environnement
# pour le projet SAE Badgeuse
# ============================================

set -e  # Arrêter en cas d'erreur

VENV_DIR=".venv"
PROJECT_DIR=$(cd "$(dirname "$0")" && pwd)

echo "======================================"
echo "  Configuration environnement projet"
echo "======================================"
echo ""

# Vérifier Python3
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 n'est pas installé"
    echo "   Installez-le avec: brew install python3"
    exit 1
fi

echo "✅ Python3 trouvé: $(python3 --version)"

# Créer l'environnement virtuel s'il n'existe pas
if [ ! -d "$VENV_DIR" ]; then
    echo ""
    echo "📦 Création de l'environnement virtuel..."
    python3 -m venv "$VENV_DIR"
    echo "✅ Environnement virtuel créé dans $VENV_DIR"
else
    echo "✅ Environnement virtuel existant trouvé"
fi

# Activer l'environnement virtuel
echo ""
echo "🔧 Activation de l'environnement virtuel..."
source "$VENV_DIR/bin/activate"

# Mettre à jour pip
echo ""
echo "📦 Mise à jour de pip..."
pip install --quiet --upgrade pip

# Installer PlatformIO dans l'environnement virtuel
echo ""
echo "📦 Installation de PlatformIO..."
if ! pip show platformio &> /dev/null; then
    pip install --quiet platformio
    echo "✅ PlatformIO installé"
else
    echo "✅ PlatformIO déjà installé"
fi

# Vérifier l'installation
PIO_VERSION=$(pio --version)
echo "✅ PlatformIO: $PIO_VERSION"

echo ""
echo "======================================"
echo "  Configuration terminée ! 🎉"
echo "======================================"
echo ""
echo "Pour utiliser l'environnement:"
echo ""
echo "1. Activez l'environnement virtuel:"
echo "   source .venv/bin/activate"
echo ""
echo "2. Compilez le projet:"
echo "   pio run -e uno_r4_wifi"
echo ""
echo "3. Téléversez sur l'Arduino:"
echo "   pio run -e uno_r4_wifi --target upload"
echo ""
echo "4. Moniteur série:"
echo "   pio device monitor -b 115200"
echo ""
echo "5. Pour quitter l'environnement:"
echo "   deactivate"
echo ""

# Créer un fichier d'activation rapide
cat > activate.sh << 'EOF'
#!/bin/bash
# Script d'activation rapide de l'environnement virtuel
source "$(dirname "$0")/.venv/bin/activate"
echo "✅ Environnement virtuel activé"
echo "   Pour compiler: pio run -e uno_r4_wifi"
echo "   Pour quitter: deactivate"
EOF

chmod +x activate.sh

echo "💡 Astuce: Utilisez './activate.sh' pour activer rapidement l'environnement"
echo ""
