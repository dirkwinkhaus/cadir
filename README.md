# cadir
If you suffer from many builds requesting the same vendor libraries
than cadir can help! It works e.g. with composer or npm and all
other package manager that have a file to identiy the current vendor 
package versions (like composer.lock or package-lock.json). cadir will 
also run the command for installing vendors. 

## Requirements
Libarchive is bundled, but a few packages are required to build and run:

* cmake: build tool for cadir
* make: build tool for cadir and libarchive
* c compiler: for libarchive
* c++ compiler: for cadir
* openssl: for hash algorithms
* zlib: for gz compression

On debian, this will install the required dependencies:

    sudo apt install gcc g++ cmake make libssl-dev zlib1g-dev

## Installation
Checkout repository

    git clone https://github.com/dirkwinkhaus/cadir.git

Build

    cd cadir
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j
    
Move or link the executable to somewhere the system finds it.

## Usage
Lets say you have a build server. On this each branch is built many times 
and the vendors will change not often. Here an example for a project with 
npm. To see which vendors in which version are needed the identity file is
"package-lock.json". On your build server you create the directory "vendorCache" 
in "/tmp". So here is the example:

    cadir --cache-source="vendor" --identity-file="composer.lock" --cache-destination="/tmp/vendorCache" --command-working-directory=`pwd` --setup="composer install --ignore-platform-reqs --no-cache --no-interaction" --finalize="composer dump-autoload --no-interaction" --verbose
   
cadir will check if a directory in the cache folder exists which has a name 
equal to the checksum of "package-lock.json". If not, it will the command in 
the working dorectory for the command which installs the npm dependencies and 
copy it to the cache folder. If cadir finds a cached copy with a fitting name
than it will copy or link it to the projekt folder.

### Identity by directoy content
By default, the identity check is made by build a has from one file. If you want to
check about a change inside a folder to create a cache for compiled or transpiled 
code you can create a file containing all hashes of each single file in the folder
and it subfolders. Than the new created file is your identity file.

## Arguments
            --cache-source                  The directory which should be cached"
            --identity-file                 File which shows differences
            --cache-destination             The directory where the cache is stored
            --command-working-directory     Working directory where the setup command is called from
            --setup                         Argument which is called if cache is not found
            --finalize                      (optional) Command which is called after cache is regenerated, linked or copied");
            -v,--verbose                    (optional) Show verbose output
            -a,--archive                    (optional) In case of copying the data a tar compressed archive (tar.gz) will be created
            -l,--link                       (optional) Link cache instead of copy
            -h,--help                       (optional) Show help
            -V,--version                    (optional) Show version
            -s,--show-cache-hit             (optional) Show if cache was hit. Even if verbose is not set, this will be displayed

## Return values
      0 = Successfully executed
      8 = Wrong usage of arguments
      9 = Identity file error (not found/no rights)
     10 = Setup command failed
     11 = Finalize command failed
     12 = Cannot copy to cache directoy
     13 = Cannot copy from cache directoy
     14 = Cannot create link from cache
     15 = Removing existing cache folder failed
     16 = Cannot create cache directories
     17 = gzip error (only with option a, archive)

# Change log
## 1.1.1        Return Codes
    changed:    return codes
    add:        information about cache hit
## 1.1.0        Archive
    add:        cache could be compressed to "tar.gz"
    add:        update time of cache at reading
## 1.0.0        First shot
    add:        whole implementation
