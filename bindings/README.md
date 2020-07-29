 # Building bindings

### Python-bindings
To build the Pythong bindings use `setup.py` in the root folder. 

    pip install wheel
    python setup.py bdist_wheel

After a successful build the bindings can be found under `<project root>/dist/`

#### Usage

Install the wheel via pip. e.g

    pip install pybiginteger<build-version>.whl

Import

    from pybiginteger import BigInteger
    print(help(BigInteger))
    
#### Wrong Python version

By default CMake will attempt to detect the correct Python executable. 

Configure example output

    -- Using Python: 
    -- Found PythonInterp: /usr/local/bin/python3.8 (found version "3.8.2") 
    -- Found PythonLibs: /usr/local/opt/python@3.8/Frameworks/Python.framework/Versions/3.8/lib/libpython3.8.dylib  

If it detects the wrong version or instance (e.g. in case you're using a virtual environment) you can set the `PYTHON_EXECUTABLE` variable to point to your desired instance.

The following example uses the virtual environment found under `<project root>/venv/` executing the commands from `<project root>/build/`. 

     cmake -D PYTHON_EXECUTABLE=../venv/bin/python3 -D BUILD_PYTHON_BINDINGS=ON ..
     cmake --build . 