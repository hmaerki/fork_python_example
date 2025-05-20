# Encoder

See:
* https://github.com/pybind/python_example
* https://cibuildwheel.readthedocs.io

## Local build and test

```bash
uv venv --python 3.13.3 .venv
. .venv/bin/activate
uv pip install -e .
python tests/test.py
```

## Local build and test

```bash
uv venv --python 3.13.3 .venv
. .venv/bin/activate
uv pip install cibuildwheel==2.23.3
./run_build_and_test.sh
```







