"""
Build stub files using just 1 interpreter (python 3.8 hardcoded)
and using a specified platform ('macosx', 'linux' or 'win')

Syntax:
    generate-stubs <wheel dir> <platform> <extension name>

Example:
    generate-stubs ./dist macosx pybiginteger
"""

import sys
import os
import subprocess
import re


def install_pkg(package_name, wheel_dir, platform):
    for file in os.listdir(wheel_dir):
        if 'cp310' in file and platform in file and file.endswith('.whl'):
            # install without dependencies, because it depends on the stub files which we're building here
            subprocess.run(['pip', 'install', wheel_dir + '/' + file, '--no-dependencies', '--force-reinstall'])
            version = re.search(f"{package_name}-(.+?)-.*", file).group(1)
            return version


def create_setup(package_name, package_version, wheel_dir):
    with open(f"{wheel_dir}/setup.py", "w") as setuppy:
        setuppy.write("""from setuptools import setup
setup(
    name='{package_name}-stubs',
    maintainer="Erik van den Brink",
    maintainer_email="erik@coz.io",
    description="PEP 561 type stubs for {package_name}",
    version='{package_version}',
    packages=['{package_name}-stubs'],
    # PEP 561 requires these
    install_requires=['{package_name}=={package_version}'],
    include_package_data=True
)
""".format(package_name=package_name, package_version=package_version))

    with open(f"{wheel_dir}/MANIFEST.in", "w") as manifest_setup:
        manifest_setup.write(f"""include {package_name}-stubs/*""")


def main():
    wheel_dir = sys.argv[1]
    platform = sys.argv[2]
    package_name = sys.argv[3]
    # prepare by installing the extension and stubgen toll
    subprocess.check_call(['python', '-m', 'pip', 'install', 'pybind11-stubgen==0.16.2'])
    package_version = install_pkg(package_name, wheel_dir, platform)
    # generate stubs
    subprocess.check_call(['pybind11-stubgen', '--skip-signature-downgrade', '--ignore-invalid=all', '--no-setup-py', '-o', '.', package_name], cwd=wheel_dir)
    # create setup.py with proper information
    create_setup(package_name, package_version, wheel_dir)
    # polish up stub files
    subprocess.check_call(['python', './tools/fix-stubs.py', f"./{wheel_dir}/{package_name}-stubs/__init__.pyi"])
    # build wheel
    subprocess.check_call(['python', 'setup.py', 'bdist_wheel', '--dist-dir', '.'], cwd=wheel_dir)


if __name__ == "__main__":
    main()
