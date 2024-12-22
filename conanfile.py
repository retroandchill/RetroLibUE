import os

from conan import ConanFile
from conan.tools.files import copy


class RetroLibUEConanFile(ConanFile):
    
    def requirements(self):
        self.requires("retrolib/0.1.0")
        
    def generate(self):
        for req, dep in self.dependencies.items():
            copy(self, "*", dst=os.path.join(self.source_folder, "Source", "ThirdParty", "RetroLib", "include"), src=os.path.join(dep.package_folder, "include"))