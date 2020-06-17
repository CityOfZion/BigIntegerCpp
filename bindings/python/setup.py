import sys
import setuptools
from distutils.core import setup

if sys.version_info < (3, 7):
    sys.exit('Python < 3.7 is not supported')

setup(
    author='Erik van den Brink (ixje), merl111',
    author_email='erik@coz.io',
    name='pybiginteger',
    description="C++ port of the C# BigInteger class",
    version='${PROJECT_VERSION}',
    packages=['pybiginteger'],
    package_dir={
        '': '${CMAKE_CURRENT_BINARY_DIR}'
    },
    package_data={
        '': ['pybiginteger.${PYTHON_MODULE_EXTENSION}${CMAKE_SHARED_LIBRARY_SUFFIX}']
    },
    classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        "Programming Language :: C++"
    ]
)
