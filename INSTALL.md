# macos

**Install system dependencies**

```
$ brew install git cmake libomp
$ export OpenMP_ROOT=$(brew --prefix)/opt/libomp 
```

**Clone project**

```
$ git clone https://github.com/stefandevai/ysamba.git
$ cd ysamba
```

**Fetch project dependencies**

```
$ git submodule update --init --recursive
$ ./lib/vcpkg/bootstrap-vcpkg.sh -disableMetrics
```

**Build project**

```
$ mkdir build && cd build
$ cmake -DCMAKE_BUILD_TYPE=Release ..
$ make
```

The compiled binary will be under `ysamba/build/bin/`.
