import os, re, gzip

class OszicarError(Exception):
    def __init__(self,msg):
        self.msg = msg

    def __str__(self):
        return self.msg


class Oszicar(object):
    """Parse OSZICAR files.

       Currently, just contains:
           self.E = list of ionic step E0 values
    """
    def __init__(self,filename):
        self.filename = filename
        self.E = []
        try:
            self.read()
        except OszicarError as e:
            raise e


    def read(self):
        """Parse OSZICAR file. Currently just collects E0 as list 'self.E'"""
        if os.path.isfile(self.filename):
            if self.filename.split(".")[-1].lower() == "gz":
                f = gzip.open(self.filename)
            else:
                f = open(self.filename)
        elif os.path.isfile(self.filename+".gz"):
            f = gzip.open(self.filename+".gz")
            raise OszicarError("file not found: " + self.filename)

        self.E = []
        for line in f:
            if re.search("E0",line):
                self.E.append(float(line.split()[4]))
        f.close()


