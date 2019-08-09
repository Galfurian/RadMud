# RadMud Editor

To use the editor you need to generate the Python code for the GUI.

To do that, you need the following programs:
 - `PyQt5`
 - `PyQt5-tools` (for the `pyuic5` program)

Once you have the required software, you just need to generate the GUI with the following command:
```shell script
pyuic5 --output=radmuddbe_gui.py radmuddbe.ui
```

Then, you can run the editor with the command:
```shell script
python radmuddbe.py
```
