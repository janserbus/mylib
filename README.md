# MyLib project

Currently, there are two libraries supported in this project:

- treerend library
- dstructs library

## Instalation

First, navigate to the directory where you want to download this repository and clone it using the following command:

```bash
git clone https://github.com/janserbus/mylib.git
```

Next, you need to install all the dependencies:

- `libsdl2-2.0-0` and `libsdl2-dev`
- `libsdl2-ttf-2.0-0` and `libsdl2-ttf-dev`
- `libsdl2-gfx-1.0-0` and `libsdl2-gfx-dev`
- `libfontconfig1` and `libfontconfig1-dev`

Update your packages:

```bash
sudo apt update
```

Then, install all the dependencies:

```bash
sudo apt install libsdl2-2.0-0 libsdl2-dev libsdl2-ttf-2.0-0 libsdl2-ttf-dev libsdl2-gfx-1.0-0 libsdl2-gfx-dev libfontconfig1 libfontconfig1-dev
```

Once you have successfully installed all the dependencies, navigate to the cloned directory and run the make script to compile and install the libraries:

```bash
cd mylib
sudo make install
```

Now, your libraries are ready for use!

## Uninstalling the Libraries

To uninstall the mylib files, run the following command:

```bash
sudo make uninstall
```

After uninstalling the library files, navigate up the directory and delete the cloned repository:

```bash
cd ..
rm -rf mylib
```

To remove all the dependencies that were installed execute the following commands:

```bash
sudo apt remove --purge libsdl2-2.0-0 libsdl2-dev libsdl2-ttf-2.0-0 libsdl2-ttf-dev libsdl2-gfx-1.0-0 libsdl2-gfx-dev libfontconfig1 libfontconfig1-dev
sudo apt autoremove --purge
```
