from conans import ConanFile


class IncbinConan(ConanFile):
    name = "incbin"
    description = "Include binary files in C/C++"
    homepage = "https://github.com/graphitemaster/incbin.git"
    version = "20180413"
    license = "Unlicense"
    scm = {
        "type": "git",
        "url": homepage,
        "revision": "c61cae60e3d47fd3d59e937693c0c4787dcc55ea",
    }

    # package info
    url = "https://github.com/rbrich/xcikit/tree/master/conan/incbin"
    author = "Radek Brich"
    generators = "cmake"

    def package(self):
        self.copy("incbin.h", dst="include")
