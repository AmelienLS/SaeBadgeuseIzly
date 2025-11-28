import os

TOOLCHAIN_DIR = os.path.expanduser("~/.platformio/packages/toolchain-gccarmnoneeabi/bin")
SYSTEM_DIRS = ["/opt/homebrew/bin", "/usr/local/bin", "/usr/bin", "/bin", "/usr/sbin", "/sbin"]

def before_build(source, target, env):
    path_parts = [TOOLCHAIN_DIR] + SYSTEM_DIRS
    env['ENV']['PATH'] = os.pathsep.join(path_parts)
