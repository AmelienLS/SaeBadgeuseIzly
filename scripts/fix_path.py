import os

Import("env")

# Configurer le PATH immédiatement
toolchain_dir = os.path.expanduser("~/.platformio/packages/toolchain-gccarmnoneeabi/bin")

# PATH minimal mais fonctionnel
path_dirs = [
    toolchain_dir,
    "/opt/homebrew/bin",
    "/usr/local/bin",
    "/usr/bin",
    "/bin",
    "/usr/sbin",
    "/sbin"
]

# Ajouter le PATH système s'il existe
system_path = os.environ.get('PATH', '')
if system_path:
    # Ajouter les répertoires système qui ne sont pas déjà dans la liste
    for path_dir in system_path.split(os.pathsep):
        if path_dir and path_dir not in path_dirs:
            path_dirs.append(path_dir)

# Configurer le PATH pour l'environnement de build
new_path = os.pathsep.join(path_dirs)
env['ENV']['PATH'] = new_path

# S'assurer que SHELL est défini
if 'SHELL' not in env['ENV']:
    env['ENV']['SHELL'] = '/bin/sh'

print("[fix_path] PATH configuré pour la compilation")
